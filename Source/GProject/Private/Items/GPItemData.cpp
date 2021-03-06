// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/GPItemData.h"

FPrimaryAssetId UGPItemData::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(ItemType, GetFName());
}