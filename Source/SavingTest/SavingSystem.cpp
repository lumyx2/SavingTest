#include "SavingSystem.h"
#include "SavingTest.h"
#include "Kismet/GameplayStatics.h"
#include "UMySerialiser.h"

void USavingSystem::LoadCreateMasterSaveRecord_WAsyncWrite(FString MasterSaveRecordFName)
{
  //bool retval = false;
  MasterRecordSaveGame = Cast<UMasterSaveGameC>(UGameplayStatics::LoadGameFromSlot(MasterSaveRecordFName, 0));
  if (MasterRecordSaveGame)
  {
    // The operation was successful, so LoadedGame now contains the data we saved earlier.
    UE_LOG(LogSavingTest, Log, TEXT("LOADED MasterSaveGame"));
  }
  else {
    UE_LOG(LogSavingTest, Log, TEXT("No MasterSaveGame! Creating new one..."));
    //create
    MasterRecordSaveGame = Cast<UMasterSaveGameC>(UGameplayStatics::CreateSaveGameObject(UMasterSaveGameC::StaticClass()));
    //write 
    HelperWriteSaveToDisk(MasterRecordSaveGame, MasterSaveRecordFileName);

  }
  //return retval;

  MasterSaveRecordFileName = MasterSaveRecordFName;
}




void USavingSystem::LoadCreateSaveGameWithName_WAsyncWrite(FString SaveNameToLoad, bool ShouldWrite)
{
  if (!MasterRecordSaveGame)
    LoadCreateMasterSaveRecord_WAsyncWrite();

  //shoulkd have master by now 
  //check 
  bool saveGameRecorded = MasterRecordSaveGame->IsSaveGameNameRecorded(SaveNameToLoad);
  if (!saveGameRecorded)
    UE_LOG(LogSavingTest, Log, TEXT("MasterSaveGame has no record of %s. Trying to check for the file..."), *SaveNameToLoad);


  CurrentActiveSaveGame = Cast<UMyGameSaveC>(UGameplayStatics::LoadGameFromSlot(SaveNameToLoad, 0));
  if (CurrentActiveSaveGame) {
    CurrentActiveSaveName = SaveNameToLoad;

    UE_LOG(LogSavingTest, Log, TEXT("LOADED %s"), *CurrentActiveSaveName);
  }
  else {
    UE_LOG(LogSavingTest, Log, TEXT("Save file for %s not found creating new save..."), *SaveNameToLoad);
    CurrentActiveSaveGame = Cast<UMyGameSaveC>(UGameplayStatics::CreateSaveGameObject(UMyGameSaveC::StaticClass()));
    CurrentActiveSaveName = SaveNameToLoad;
    UpdateSaveTimeInSaveGameAndMasterRecord(CurrentActiveSaveName);
    if (ShouldWrite)
      HelperWriteSaveToDisk(CurrentActiveSaveGame, SaveNameToLoad);

  }


}


void USavingSystem::HelperWriteSaveToDisk(USaveGame* SaveToSave, FString SlotNameString, bool UseAsync) {


  if (UseAsync) {
    //TODO not implemented properly
    // Set up the (optional) delegate.
    FAsyncSaveGameToSlotDelegate SavedDelegate;
    //USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
    SavedDelegate.BindUObject(this, &USavingSystem::SaveGameDelegateFunction);


    // Start async save process.
    UGameplayStatics::AsyncSaveGameToSlot(SaveToSave, SlotNameString, 0, SavedDelegate);
  }
  else {
    UGameplayStatics::SaveGameToSlot(SaveToSave, SlotNameString, 0);
  }
}






void USavingSystem::UpdateSaveTimeInSaveGameAndMasterRecord(FString SaveGameName) {
  //update master and
  auto saveRecord = HelperUpdateSavedTimesEtc(SaveGameName);
  MasterRecordSaveGame->AddUpdateRecordForSave(SaveGameName, saveRecord);
  CurrentActiveSaveGame->MasterSaveRecordCopy = saveRecord;
}



UObject* USavingSystem::DeserialiseMasterUObjectForCurrentActiveSaveName(UObject* InOuter) {
  UObject* retval = nullptr;
  //must correspond to current active save! 
  if (!CurrentActiveSaveGame) {
    UE_LOG(LogSavingTest, Log, TEXT("No Save game loaded!"));
    return retval;
  }

  retval = UMySerialiser::DeserializeUObjectFromByteArray(CurrentActiveSaveGame->SerialisedDataMasterObject, InOuter);
  return retval;
}


UObject* USavingSystem::DeserialiseUObjectForCurrentActiveSaveName_ExternalFile(FString UObjectName, UObject* InOuter)
{
  UObject* retval = nullptr;
  //must correspond to current active save! 
  if (!CurrentActiveSaveGame) {
    UE_LOG(LogSavingTest, Log, TEXT("No Save game loaded!"));
    return retval;
  }

  //file record
  if (!CurrentActiveSaveGame->IsUObjectNameInRelatedExternalFiles(UObjectName)) {
    UE_LOG(LogSavingTest, Log, TEXT("No UObject with name %s!"), *UObjectName);
    return retval;
  }
  //do it
  retval = UMySerialiser::DeserializeUObject(HelperGetUObjectSavedName(UObjectName, CurrentActiveSaveName), InOuter);

  return retval;
}


FString USavingSystem::HelperGetUObjectSavedName(FString UObjectName, FString SaveGameName) {
  //FString retval = FString::Printf(TEXT("%s_%s%s"), *CurrentActiveSaveName, UObjectName, DEFAULT_DEF_UOBJSUBFIX);
  FString retval = SaveGameName + "_" + UObjectName + DEFAULT_DEF_UOBJSUBFIX;
  return retval;
}





UObject* USavingSystem::DeserialiseUObjectForSaveName_ExternalFile(FString SaveNameToLoad, FString UObjectName, UObject* InOuter)
{
  UObject* retval = nullptr;
  retval = UMySerialiser::DeserializeUObject(HelperGetUObjectSavedName(UObjectName, SaveNameToLoad), InOuter);

  if (!retval)
    UE_LOG(LogSavingTest, Log, TEXT("No UObject with name %s for save %s!"), *UObjectName, *SaveNameToLoad);

  return retval;
}




void USavingSystem::SaveMasterUObjectToSaveGameWithName_WAsyncWrite(FString SaveNameToLoad, UObject* obj)
{

  //the master will be crated if it doesn't exist 
  if (!CurrentActiveSaveGame) {
    //try to load 
    LoadCreateSaveGameWithName_WAsyncWrite(SaveNameToLoad, false);
  }
  //UpdateSaveTimeInSaveGameAndMasterRecord()
    //needs to create game and add it to master 
  UMySerialiser::SerializeObjectToByteArray(obj, CurrentActiveSaveGame->SerialisedDataMasterObject);
  //update and write
  UpdateSaveTimeInSaveGameAndMasterRecord(CurrentActiveSaveName);
  HelperWriteSaveToDisk(CurrentActiveSaveGame, SaveNameToLoad);
  HelperWriteSaveToDisk(MasterRecordSaveGame, MasterSaveRecordFileName);
}


bool USavingSystem::SerialiseUObjecForCurrentSaveToExternalFile_WAsyncWrite(FString UObjectName, UObject* InObj)
{
  //the master will be crated if it doesn't exist 
  if (!CurrentActiveSaveGame) {
    return false;
  }
  //record
  CurrentActiveSaveGame->ExternallySerialisedUObjectNames.Emplace(UObjectName);
  //save object 
  UMySerialiser::SerializeUObject(InObj, HelperGetUObjectSavedName(UObjectName, CurrentActiveSaveName));
  //update and write
  UpdateSaveTimeInSaveGameAndMasterRecord(CurrentActiveSaveName);
  HelperWriteSaveToDisk(CurrentActiveSaveGame, CurrentActiveSaveName);
  HelperWriteSaveToDisk(MasterRecordSaveGame, MasterSaveRecordFileName);
  return true;
}

void USavingSystem::SaveGameDelegateFunction(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
  //TODO
}




FSCPP_SavingRecord USavingSystem::HelperUpdateSavedTimesEtc(FString SaveGameName) {
  FSCPP_SavingRecord newRecord;

  newRecord.SaveName = SaveGameName;
  newRecord.LastTimePlayed = FDateTime::Now();
  newRecord.LastTimePlayed_UTC = FDateTime::UtcNow();
  newRecord.LastTimePlayedStr = newRecord.LastTimePlayed.ToIso8601();

  return newRecord;
}