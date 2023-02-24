#pragma once
#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Containers/UnrealString.h"


struct FSerializedObject
{
  UClass* Class;
  FString OuterPath;
  TArray<uint8> Bytes;
};

struct FObjectSnapshot
{
  /** The object we are serializing (the "root") */
  FSerializedObject Object;
  /** All subobjects (recursive hierarchy included) */
  TMap<FString, FSerializedObject> Subobjects;
  /** Remember class references so we use LoadClass on them */
  TSet<FString> Classes;
};

static FArchive& operator<<(FArchive& Ar, FSerializedObject& Obj)
{
  Ar << Obj.Class;
  Ar << Obj.OuterPath;
  Ar << Obj.Bytes;
  return Ar;
}
static FArchive& operator<<(FArchive& Ar, FObjectSnapshot& Snapshot)
{
  Ar << Snapshot.Object;
  Ar << Snapshot.Subobjects;
  Ar << Snapshot.Classes;
  return Ar;
}

struct FSharedArchiveData
{
  UObject* RootObj;
  AActor* OuterActor;

  FObjectSnapshot Snapshot;
  FString RootPath;

  // When writing
  TMap<UObject*, FString> VisitedObjects;
  // When reading
  TMap<FString, UObject*> DeserializedObjects;
};

