#pragma once
#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "UObject/NoExportTypes.h"
#include "MasterSaveGameC.h"
#include "Containers/UnrealString.h"
#include "SavingSystem.generated.h"


#define DEFAULT_MASTERSAVE_FNAME "MasterSaveFile"
#define DEFAULT_DEF_UOBJSUBFIX "_Data"


//const FString DEFAULT_MASTERSAVE_FNAME = TEXT("MasterSaveFile");

UCLASS(Blueprintable, BlueprintType)
class SAVINGTEST_API USavingSystem : public UObject

{

  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CurrentActiveSaveName = "";


  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMasterSaveGameC* MasterRecordSaveGame;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMyGameSaveC* CurrentActiveSaveGame;


  /**
  * Load functions
  */




  //TODO you need to have only a load(that when it craetes writesand a save that always writes(for either the savefile or the uobject added))



  //UFUNCTION(BlueprintCallable)
  //  void LoadCreateMasterSaveRecord_NoWrite(FString MasterSaveRecordFName = "MasterSaveFile");

  //UFUNCTION(BlueprintCallable)
  //  void LoadCreateSaveGameWithName_NoWrite(FString SaveNameToLoad);

  UFUNCTION(BlueprintCallable)
    void LoadCreateMasterSaveRecord_WAsyncWrite(FString MasterSaveRecordFName = "MasterSaveFile");

  UFUNCTION(BlueprintCallable)
    void LoadCreateSaveGameWithName_WAsyncWrite(FString SaveNameToLoad, bool ShouldWrite = true);



  /**
  * Files saved inside save
  */
  UFUNCTION(BlueprintCallable)
    UObject* DeserialiseMasterUObjectForCurrentActiveSaveName(UObject* InOuter);



  /**
  * Files saved separately
  */

  UFUNCTION(BlueprintCallable)
    UObject* DeserialiseUObjectForCurrentActiveSaveName_ExternalFile(FString UObjectName, UObject* InOuter);


  UFUNCTION(BlueprintCallable)
    UObject* DeserialiseUObjectForSaveName_ExternalFile(FString SaveNameToLoad, FString UObjectName, UObject* InOuter);


  /**
  * Save functions
  */
  UFUNCTION(BlueprintCallable)
    void SaveMasterUObjectToSaveGameWithName_WAsyncWrite(FString SaveNameToLoad, UObject* obj);

  UFUNCTION(BlueprintCallable)
    bool SerialiseUObjecForCurrentSaveToExternalFile_WAsyncWrite(FString UObjectName, UObject* InObj);

  //TODO
  void SaveGameDelegateFunction(const FString& SlotName, const int32 UserIndex, bool bSuccess);

private:

  UPROPERTY()
    FString MasterSaveRecordFileName = DEFAULT_MASTERSAVE_FNAME;


  UFUNCTION()
    FString HelperGetUObjectSavedName(FString UObjectName, FString SaveGameName);

  UFUNCTION()
    FSCPP_SavingRecord HelperUpdateSavedTimesEtc(FString SaveGameName);

  UFUNCTION()
    void HelperWriteSaveToDisk(USaveGame* SaveToSave, FString SlotNameString, bool UseAsync = false);

  UFUNCTION()
    void UpdateSaveTimeInSaveGameAndMasterRecord(FString SaveGameName);


  //UFUNCTION()
  //  void WriteToDiskMasterRecordAndThisSaveFile(FString SaveGameName);

};

