

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "SaveDefs.h"
#include "UMySerialiser.generated.h"


UCLASS()
class SAVINGTEST_API UMySerialiser : public UBlueprintFunctionLibrary
{

  GENERATED_BODY()

public:

  UFUNCTION(BlueprintCallable)
    static void SerializeObjectToByteArray(UObject* Obj, TArray<uint8>& FinalBytes);


  UFUNCTION(BlueprintCallable)
    static void SerializeUObject(UObject* Obj, FString FileName);



  /////////////////////////


  UFUNCTION(BlueprintCallable)
    static UObject* DeserializeUObject(FString FileName, UObject* InOuter);

  UFUNCTION(BlueprintCallable)
    static UObject* DeserializeUObjectFromByteArray(TArray<uint8>& AllBytes, UObject* InOuter);
};