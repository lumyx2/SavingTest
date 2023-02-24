// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Containers/Array.h"
#include "Misc/DateTime.h" 
#include "Engine/GameInstance.h" 
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Containers/UnrealString.h"
#include "SaveDefs.h"

#include "MasterSaveGameC.generated.h"







struct FObjectSnapshotProxyArchive : public FNameAsStringProxyArchive
{
  FSharedArchiveData& Shared;

  FObjectSnapshotProxyArchive(FArchive& InnerAr, FSharedArchiveData& Shared) : FNameAsStringProxyArchive(InnerAr), Shared(Shared)
  {
    ArNoDelta = true;	//preferred
    ArIsSaveGame = true;	//optional
  }

  static void DeserializeObject(UObject*& Obj, FSharedArchiveData& Shared, const FString& SerializedPath, FSerializedObject& Data, UObject* Outer, bool bIsRoot)
  {
    UActorComponent* ComponentToRegister = nullptr;
    if (!Obj)
    {
      if (Shared.OuterActor && Data.Class->IsChildOf<UActorComponent>())
      {
        Obj = ComponentToRegister = Shared.OuterActor->AddComponentByClass(Data.Class, true, FTransform::Identity, true);
      }
      else
      {
        FStaticConstructObjectParameters Parms(Data.Class);
        Parms.Outer = Outer;
        Obj = StaticConstructObject_Internal(Parms);
      }
    }

    UE_LOG(LogTemp, Log, TEXT("[DeserializeObject] Deserializing into %s"), *SerializedPath);

    if (bIsRoot)
      Shared.RootPath = Obj->GetPathName();

    // Don't deserialize this object again
    Shared.DeserializedObjects.Add(SerializedPath, Obj);

    // Deserialize into the found/created object - this will recursively deserialize subobjects referenced by UProperties
    FMemoryReader ObjectReader(Data.Bytes, true);
    FObjectSnapshotProxyArchive ObjectAr(ObjectReader, Shared);
    Obj->Serialize(ObjectAr);

    UE_LOG(LogTemp, Log, TEXT("[DeserializeObject] Deserialized %s"), *SerializedPath);

    // Register new component if applicable
    if (ComponentToRegister)
    {
      if (auto SceneComp = Cast<USceneComponent>(ComponentToRegister))
        Shared.OuterActor->FinishAddComponent(ComponentToRegister, true, SceneComp->GetRelativeTransform());
      else
        Shared.OuterActor->FinishAddComponent(ComponentToRegister, true, FTransform::Identity);
    }
  }

  UObject* DeserializeObjectPath(const FString& SerializedPath)
  {
    // Special case for serialized class refs : use LoadObject instead of FindObject
    if (Shared.Snapshot.Classes.Contains(SerializedPath))
    {
      return LoadObject<UClass>(nullptr, *SerializedPath);
    }
    // Already visited this
    else if (auto KnownObj = Shared.DeserializedObjects.Find(SerializedPath))
    {
      return *KnownObj;
    }
    else if (auto Data = Shared.Snapshot.Subobjects.Find(SerializedPath))
    {
      // Recursively ensure Outer is deserialized first
      UObject* Outer = DeserializeObjectPath(Data->OuterPath);
      if (!Outer)
      {
        UE_LOG(LogTemp, Warning, TEXT("Could not find or deserialize Outer for %s (Outer path = %s)"), *SerializedPath, *Data->OuterPath);
        return nullptr;
      }

      // Deserializing Outer could have created us
      if (auto KnownObj2 = Shared.DeserializedObjects.Find(SerializedPath))
      {
        return *KnownObj2;
      }

      UObject* Obj = nullptr;

      // We might already exist as a default subobject (component)
      FString FullPath = SerializedPath.Replace(TEXT("$ROOT$"), *Shared.RootPath);
      if (FullPath.StartsWith(Outer->GetPathName()))
      {
        Obj = FindObject<UObject>(nullptr, *FullPath, false);
        // Class check
        if (Obj && Obj->GetClass() != Data->Class)
          Obj = nullptr;
      }

      DeserializeObject(Obj, Shared, SerializedPath, *Data, Outer, false);
      return Obj;
    }
    else if (SerializedPath.StartsWith("$ROOT$"))
    {
      UE_LOG(LogTemp, Warning, TEXT("Found $ROOT$ ref not in Subobjects nor in DeserializedObjects: %s"), *SerializedPath);
      return nullptr;
    }
    else
    {
      return FindObject<UObject>(nullptr, *SerializedPath, false);
    }
  }

  FString GetSerializeObjectPath(UObject* Obj)
  {
    FString Path = Obj->GetPathName();
    if (Path.StartsWith(Shared.RootPath))
      Path = FString("$ROOT$") + Path.Mid(Shared.RootPath.Len());
    return Path;
  }

  virtual FArchive& operator<<(UObject*& Obj) override
  {
    if (IsLoading())
    {
      FString SerializedPath;
      InnerArchive << SerializedPath;
      Obj = DeserializeObjectPath(SerializedPath);
    }
    else
    {
      // Check if we already visited this
      if (auto SerializedPath = Shared.VisitedObjects.Find(Obj))
      {
        InnerArchive << *SerializedPath;
      }
      else if (Obj)
      {
        FString Path = Obj->GetPathName();

        if (Obj->IsA<UClass>())	// class special case
        {
          Shared.Snapshot.Classes.Add(Path);
        }
        else if (Path.StartsWith(Shared.RootPath))	//not sure if we should check Outer chain or this
        {
          // Generic opt-in event
          if (UFunction* Func = Obj->FindFunction("OnSerialize"))
            Obj->ProcessEvent(Func, nullptr);

          Path = GetSerializeObjectPath(Obj);
          Shared.VisitedObjects.Add(Obj, Path);

          FSerializedObject& Data = Shared.Snapshot.Subobjects.Emplace(Path, FSerializedObject());
          Data.Class = Obj->GetClass();
          Data.OuterPath = GetSerializeObjectPath(Obj->GetOuter());

          // Serialize sub object in its own archive
          FMemoryWriter SubobjectWriter(Data.Bytes, true);
          FObjectSnapshotProxyArchive SubobjectAr(SubobjectWriter, Shared);
          Obj->Serialize(SubobjectAr);
        }
        else
        {
          // Remove Level part from path
          // Not sure if this is a good idea, external refs should probably be nulled if we change level..
          Path = Obj->GetPathName(Obj->GetTypedOuter<ULevel>());
        }

        InnerArchive << Path;

        Shared.VisitedObjects.Add(Obj, Path);
      }
      else
      {
        FString Path = TEXT("None");
        InnerArchive << Path;
        Shared.VisitedObjects.Add(Obj, Path);
      }
    }
    return *this;
  }
};

/**
Struct representing the info necessary to show the Load Screen (SaveName, hours played, etc)
*/

USTRUCT(BlueprintType)
struct FSCPP_SavingRecord //: public FTableRowBase
{
  GENERATED_BODY()

    FSCPP_SavingRecord() {}

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SaveName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LastTimePlayedStr;


  //UPROPERTY(EditAnywhere, BlueprintReadWrite)
  //  int VersionNumber = 1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastTimePlayed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastTimePlayed_UTC;
};


UCLASS(Blueprintable, BlueprintType)
class SAVINGTEST_API UMyGameSaveC : public USaveGame
{
  GENERATED_BODY()

public:
  //Same info duplicated inside the master save! 
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSCPP_SavingRecord MasterSaveRecordCopy;


  //internally serialised object and subobjects 
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<uint8> SerialisedDataMasterObject;

  //list of strings that relate to the externally serialised objects into files
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSet<FString> ExternallySerialisedUObjectNames;

  bool IsUObjectNameInRelatedExternalFiles(FString& UObjectName) {
    return ExternallySerialisedUObjectNames.Contains(UObjectName);
  }




};



/**
Contains a list of all saves record
*/
UCLASS(Blueprintable)
class SAVINGTEST_API UMasterSaveGameC : public USaveGame
{
  GENERATED_BODY()

public:

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FSCPP_SavingRecord> AllSaveRecordsBySaveName;


  bool IsSaveGameNameRecorded(FString SGName) {
    return AllSaveRecordsBySaveName.Contains(SGName);
  }

  void AddUpdateRecordForSave(FString SaveGameName, FSCPP_SavingRecord SavingRecord) {
    if (!IsSaveGameNameRecorded(SaveGameName))
      AllSaveRecordsBySaveName.Emplace(SaveGameName, SavingRecord);
    else
      AllSaveRecordsBySaveName[SaveGameName] = SavingRecord;
  }

  void DeleteRecordForSave(FString SaveGameName) {
    AllSaveRecordsBySaveName.Remove(SaveGameName);
  }

};

