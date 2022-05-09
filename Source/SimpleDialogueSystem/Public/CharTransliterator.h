// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#if UE_ENABLE_ICU

#ifdef UCONFIG_NO_TRANSLITERATION
#undef UCONFIG_NO_TRANSLITERATION
#define UCONFIG_NO_TRANSLITERATION 0
#endif

THIRD_PARTY_INCLUDES_START
#include <unicode/utrans.h>
#include <unicode/translit.h>
THIRD_PARTY_INCLUDES_END

class SIMPLEDIALOGUESYSTEM_API CharTransliterator
{
public:
	CharTransliterator();

	~CharTransliterator();

public:

	UPROPERTY()
		icu::Transliterator* BaseTransliterator = nullptr;

public:

	FString TransliterateChar(const TCHAR Char);

	FString TransliterateString(const FString Str);
};

#endif