#include "UMySerialiser.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SavingTest.h"
#include "MasterSaveGameC.h"


void UMySerialiser::SerializeObjectToByteArray(UObject* Obj, TArray<uint8>& FinalBytes) {
  // Generic opt-in event
  if (UFunction* Func = Obj->FindFunction("OnSerialize"))
    Obj->ProcessEvent(Func, nullptr);

  FSharedArchiveData Shared;
  Shared.RootObj = Obj;
  Shared.Snapshot.Object.Class = Obj->GetClass();
  Shared.RootPath = Obj->GetPathName();
  Shared.VisitedObjects.Add(Obj, TEXT("$ROOT$"));	//add root obj ref here so we don't try to serialize it as a subobject


  // Reserve container to avoid reallocation
  TArray<UObject*> Subobjects;
  GetObjectsWithOuter(Obj, Subobjects);
  Shared.Snapshot.Subobjects.Reserve(Subobjects.Num());


  // Serialize root object - subobjects will be recursively serialized into Snapshot.Subobjects
  FMemoryWriter ObjectWriter(Shared.Snapshot.Object.Bytes, true);
  FObjectSnapshotProxyArchive ObjectAr(ObjectWriter, Shared);
  Obj->Serialize(ObjectAr);

  // Serialize intermediate structure

  FMemoryWriter FinalWriter(FinalBytes, true);
  FObjectAndNameAsStringProxyArchive FinalAr(FinalWriter, true);
  FinalAr << Shared.Snapshot;
}

void UMySerialiser::SerializeUObject(UObject* Obj, FString FileName)
{
  TArray<uint8> FinalBytes;
  SerializeObjectToByteArray(Obj, FinalBytes);
  // Save to file
  FString FilePath = FPaths::ProjectSavedDir() / FileName;
  bool retval = FFileHelper::SaveArrayToFile(FinalBytes, *FilePath);
  if (retval) {
    UE_LOG(LogSavingTest, Log, TEXT("Saved to %s!"), *FileName);
  }
  else {
    UE_LOG(LogSavingTest, Log, TEXT("Failed to save to %s!"), *FileName);

  }

}


//Note: If deserialising actors then the InOuter must be a world object!!
UObject* UMySerialiser::DeserializeUObjectFromByteArray(TArray<uint8>& AllBytes, UObject* InOuter) {
  // Deserialize structure
  FMemoryReader StructuralReader = FMemoryReader(AllBytes, true);
  FObjectAndNameAsStringProxyArchive StructuralAr(StructuralReader, true);
  FSharedArchiveData Shared;
  StructuralAr << Shared.Snapshot;

  // Spawn root object (and its default subobjects)
  if (Shared.Snapshot.Object.Class->IsChildOf<AActor>())
  {
    FActorSpawnParameters Params;
    Params.bNoFail = true;
    //Params.bDeferConstruction = true;
    Shared.RootObj = Shared.OuterActor = InOuter->GetWorld()->SpawnActor<AActor>(Shared.Snapshot.Object.Class, Params);
    //Actor->FinishSpawning(Actor->GetTransform());
  }
  else
  {
    // Non-Actor case is now factorized in FObjectSnapshotProxyArchive::DeserializeObject
    Shared.RootObj = nullptr;
    Shared.OuterActor = Cast<AActor>(InOuter);
  }

  if (!InOuter)
    InOuter = GetTransientPackage();

  // Deserialize root object - subobjects will be recursively deserialized from Snapshot.Subobjects
  FObjectSnapshotProxyArchive::DeserializeObject(Shared.RootObj, Shared, "$ROOT$", Shared.Snapshot.Object, InOuter, true);

  // Delete subobjects that were not in the snapshot (ie. default components removed at runtime)
  TArray<UObject*> Subobjects;
  GetObjectsWithOuter(Shared.RootObj, Subobjects);
  TArray<UObject*> VisitedObjects;
  Shared.DeserializedObjects.GenerateValueArray(VisitedObjects);
  for (auto Obj : Subobjects)
  {
    if (Obj && !VisitedObjects.Contains(Obj))
    {
      if (auto Comp = Cast<UActorComponent>(Obj))
        Comp->DestroyComponent();
      else
        Obj->MarkAsGarbage();
    }
  }

  // Fix up actor components
  if (Shared.RootObj == Shared.OuterActor)
    Shared.OuterActor->UpdateComponentTransforms();

  // Generic opt-in event
  for (auto& Pair : Shared.DeserializedObjects)
  {
    if (UFunction* Func = Pair.Value->FindFunction("OnDeserialize"))
      Pair.Value->ProcessEvent(Func, nullptr);
  }

  return Shared.RootObj;
}

UObject* UMySerialiser::DeserializeUObject(FString FileName, UObject* InOuter)
{
  // Read from file
  TArray<uint8> AllBytes;
  FString FilePath = FPaths::ProjectSavedDir() / FileName;
  if (!FFileHelper::LoadFileToArray(AllBytes, *FilePath)) {
    UE_LOG(LogSavingTest, Log, TEXT("Failed to open file %s"), *FileName);
    return nullptr;
  }

  return DeserializeUObjectFromByteArray(AllBytes, InOuter);
}