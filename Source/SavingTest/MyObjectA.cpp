

#include "MyObjectA.h"


void UMyObjectBIG::PrintContent() {
  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----UMyObjectBIG------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----UMyObjectBIG------"));

  //plainStruct.PrintContent();
  //ComplexStruct.PrintContent();
  //if (ObjASub != nullptr)
  //  ObjASub->PrintContent();
  //if (ObjBBSub != nullptr)
  //  ObjBBSub->PrintContent();

  //for (auto& oo : ObjBBSubArray) {
  //  if (oo != nullptr)
  //    oo->PrintContent();
  //}

  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----ObjASubArray------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----ObjASubArray------"));
  for (auto& oo : ObjASubArray) {
    if (oo != nullptr)
      oo->PrintContent();
  }
  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----ObjASubArray------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----ObjASubArray------"));

  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----FirstObjAPtr------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----FirstObjAPtr------"));
  FirstObjAPtr->PrintContent();
  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----FirstObjAPtr------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----FirstObjAPtr------"));


  auto tt = TEXT("+++----UMyObjectBIG------");
  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, tt);
  UE_LOG(LogSavingTest, Log, TEXT("+++----UMyObjectBIG------"));

}

void UMyObjectBIG::InitWithDummy() {
  /* plainStruct.InitWithDummy(55, "UMyObjectBIG");
   ComplexStruct.InitWithDummy("UMyObjectBIG");

   ObjASub = NewObject<UMyObjectA>(this);
   ObjASub->InitWithDummy(55, "UMyObjectBIG");

   ObjBBSub = NewObject<UMyObjectBB>(this);
   ObjBBSub->InitWithDummy(55, "UMyObjectBIG");*/

   //ObjBBSubArray.Add(NewObject<UMyObjectBB>(this));
   ////ObjBBSubArray.Add(NewObject<UMyObjectBB>());
   //ObjBBSubArray[0]->InitWithDummy(55, "UMyObjectBIG");
   //ObjBBSubArray[1]->InitWithDummy();

  ObjASubArray.Add(NewObject<UMyObjectA>(this));
  //ObjBBSubArray.Add(NewObject<UMyObjectBB>());
  ObjASubArray[0]->InitWithDummy(55, "UMyObjectBIG");
  ObjASubArray.Add(NewObject<UMyObjectA>(this));
  //ObjBBSubArray.Add(NewObject<UMyObjectBB>());
  ObjASubArray[1]->InitWithDummy(99, "UMyObjectBIG");


  //set up the pointer 
  FirstObjAPtr = ObjASubArray[0];

}






void UMyObjectA::InitWithDummy(int val, FString sub) {
  plainStruct.InitWithDummy(val, "UMyObjectAplain" + sub);
  ComplexStruct.InitWithDummy("UMyObjectAcomplex" + sub);

  ObjBB = NewObject<UMyObjectBB>(this);
  ObjBB->InitWithDummy(val, sub);

}


void UMyObjectA::PrintContent() {
  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----UMyObjectA------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----UMyObjectA------"));
  plainStruct.PrintContent();
  ComplexStruct.PrintContent();
  if (ObjBB)
    ObjBB->PrintContent();
  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----UMyObjectA------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----UMyObjectA------"));

}


void UMyObjectBB::PrintContent() {
  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----UMyObjectBB------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----UMyObjectBB------"));
  plainStruct.PrintContent();
  ComplexStruct.PrintContent();
  GEngine->AddOnScreenDebugMessage(-1, DISPLAY_TIME, FColor::Red, TEXT("+++----UMyObjectBB------"));
  UE_LOG(LogSavingTest, Log, TEXT("+++----UMyObjectBB------"));


}

void UMyObjectBB::InitWithDummy(int val, FString sub) {
  plainStruct.InitWithDummy(val, "UMyObjectBBplain" + sub);
  ComplexStruct.InitWithDummy("UMyObjectBBcomplex" + sub);
}