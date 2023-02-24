// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/DataTable.h"
#include "UObject/Class.h"
#include "Misc/DateTime.h"
#include "SavingTest.h"

#include "GenericPlatform/GenericPlatformMath.h"


#include "MyObjectA.generated.h"


#define DISPLAY_TIME 7.0f

UENUM(BlueprintType)
enum class E_CPP_LevelName : uint8
{
  Level_Unknown UMETA(DisplayName = "Level_Unknown"),
  Level_1 UMETA(DisplayName = "Level_1"),
  Level_2 UMETA(DisplayName = "Level_2"),
  Level_3 UMETA(DisplayName = "Level_3"),
  Level_4 UMETA(DisplayName = "Level_4"),
  Level_5 UMETA(DisplayName = "Level_5"),
  Level_6 UMETA(DisplayName = "Level_6"),
  Level_7 UMETA(DisplayName = "Level_7"),
  Level_8 UMETA(DisplayName = "Level_8"),
  //Level_All UMETA(DisplayName = "Level_All"), //for total

};



USTRUCT(BlueprintType)
struct FSCPP_Plain : public FTableRowBase
{
  GENERATED_BODY()

    FSCPP_Plain() {}


  FSCPP_Plain(int val, FString ff)
    : testInt(val),
    testStr(ff)
  { }

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    int testInt = -1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FString testStr = "notSet";


  void InitWithDummy(int val, FString ss) {
    testInt = val;
    testStr = "FSCPP_Plain" + ss;
  }


  void PrintContent() {
    if (GEngine)
    {
      GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("----FSCPP_Plain------"));
      UE_LOG(LogSavingTest, Log, TEXT("-----FSCPP_Plain-----"));

      GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Yellow, FString::Printf(TEXT("testInt %i"), testInt));
      UE_LOG(LogSavingTest, Log, TEXT("testInt %i"), testInt);

      GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Yellow, FString::Printf(TEXT("testStr %s"), *testStr));
      UE_LOG(LogSavingTest, Log, TEXT("testStr %s"), *testStr);

      GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("------FSCPP_Plain----"));
      UE_LOG(LogSavingTest, Log, TEXT("----FSCPP_Plain------"));

    }

  }

};


USTRUCT(BlueprintType)
struct FSCPP_ComplexStruct : public FTableRowBase
{
  GENERATED_BODY()

    FSCPP_ComplexStruct() {
    ArrayEnum = TArray< E_CPP_LevelName>();
    PlainArray = TArray< FSCPP_Plain>();
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    TArray< E_CPP_LevelName> ArrayEnum;


  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    TArray< FSCPP_Plain> PlainArray;


  void InitWithDummy(FString subPlain) {
    for (int i = 0; i < 1; i++) {
      PlainArray.Add(FSCPP_Plain(i, "FSCPP_Plain" + subPlain));

    }

    ArrayEnum = { {E_CPP_LevelName::Level_3}, {E_CPP_LevelName::Level_3}, {E_CPP_LevelName::Level_3} };
  }


  void PrintContent() {
    if (GEngine)
    {
      GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("----FSCPP_ComplexStruct------"));
      UE_LOG(LogSavingTest, Display, TEXT("-----FSCPP_ComplexStruct-----"));

      for (auto& ee : ArrayEnum) {
        GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Yellow, FString::Printf(TEXT("ArrayEnum %i"), ee));
        UE_LOG(LogSavingTest, Display, TEXT("ArrayEnum %i"), ee);

      }

      for (auto& ee : PlainArray) {
        ee.PrintContent();
      }
      GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("-----FSCPP_ComplexStruct-----"));
      UE_LOG(LogSavingTest, Display, TEXT("-----FSCPP_ComplexStruct-----"));

    }
  }
};


UCLASS(Blueprintable, BlueprintType)
class SAVINGTEST_API UMyObjectBB : public UObject
{
  GENERATED_BODY()

public:

  UMyObjectBB() {}

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FSCPP_Plain plainStruct;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FSCPP_ComplexStruct ComplexStruct;

  UFUNCTION(BlueprintCallable)
    void InitWithDummy(int val, FString sub);

  UFUNCTION(BlueprintCallable)
    void PrintContent();


};


/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class SAVINGTEST_API UMyObjectA : public UObject
{
  GENERATED_BODY()


public:
  UMyObjectA() {}


  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FSCPP_Plain plainStruct;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FSCPP_ComplexStruct ComplexStruct;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    UMyObjectBB* ObjBB;

  UFUNCTION(BlueprintCallable)
    void PrintContent();

  UFUNCTION(BlueprintCallable)
    void InitWithDummy(int val, FString sub);
};


UCLASS(Blueprintable, BlueprintType)
class SAVINGTEST_API UMyObjectBIG : public UObject
{
  GENERATED_BODY()

public:

  UMyObjectBIG() {
    //ObjBBSubArray = TArray<UMyObjectBB*>();
    ObjASubArray = TArray<UMyObjectA*>();
  }


  //UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
  //  FSCPP_Plain plainStruct;

  //UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
  //  FSCPP_ComplexStruct ComplexStruct;

  //UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
  //  UMyObjectA* ObjASub;


  //UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
  //  UMyObjectBB* ObjBBSub;

  //UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
  //  TArray<UMyObjectBB*> ObjBBSubArray;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    UMyObjectA* FirstObjAPtr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    TArray<UMyObjectA*> ObjASubArray;

  UFUNCTION(BlueprintCallable)
    void PrintContent();

  UFUNCTION(BlueprintCallable)
    void InitWithDummy();



};




UCLASS(Blueprintable, BlueprintType)
class SAVINGTEST_API UMasterStateObj : public UObject
{
  GENERATED_BODY()

public:

  UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    UMyObjectBIG* ObjectBig;


  UMasterStateObj() {

  }

  UFUNCTION(BlueprintCallable)
    void Init() {
    ObjectBig = NewObject<UMyObjectBIG>(this);
    //    ObjectBig = NewObject<UMyObjectBIG>(this);

    ObjectBig->InitWithDummy();
  }

  UFUNCTION(BlueprintCallable)
    void PrintContent() {
    GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----UMasterStateObj------"));
    UE_LOG(LogSavingTest, Log, TEXT("+++----UMasterStateObj------"));
    if (ObjectBig)
      ObjectBig->PrintContent();
    GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----UMasterStateObj------"));
    UE_LOG(LogSavingTest, Log, TEXT("+++----UMasterStateObj------"));
  }

};