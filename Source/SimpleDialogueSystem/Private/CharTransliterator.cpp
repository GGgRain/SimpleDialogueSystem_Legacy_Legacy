// Fill out your copyright notice in the Description page of Project Settings.


#include "CharTransliterator.h"
#include <iostream>

#if UE_ENABLE_ICU


THIRD_PARTY_INCLUDES_START
#include <unicode/utrans.h>
#include <unicode/translit.h>
THIRD_PARTY_INCLUDES_END

CharTransliterator::CharTransliterator()
{

	icu::UnicodeString TestString("Any-en_US; Latin-ASCII");

	UErrorCode status = U_ZERO_ERROR;
	BaseTransliterator = icu::Transliterator::createInstance(TestString, UTRANS_FORWARD, status);

}



CharTransliterator::~CharTransliterator()
{
	if(BaseTransliterator != nullptr) {
		icu::Transliterator::unregister(BaseTransliterator->getID());

		if (BaseTransliterator) {
			delete BaseTransliterator;
		}
	}
}

FString CharTransliterator::TransliterateChar(const TCHAR Char) {
	icu::UnicodeString Output = icu::UnicodeString(Char);

	if (BaseTransliterator != nullptr) {
		BaseTransliterator->transliterate(Output);
	}
	std::string ConvertStr; 
	Output.toUTF8String(ConvertStr);
	FString String = FString(ConvertStr.c_str());

	return String;
}


FString CharTransliterator::TransliterateString(const FString Str) {
	icu::UnicodeString Output = icu::UnicodeString(*Str);

	if (BaseTransliterator != nullptr) {
		BaseTransliterator->transliterate(Output);
	}
	std::string ConvertStr;
	Output.toUTF8String(ConvertStr);
	FString String = FString(ConvertStr.c_str());

	return String;
}

#endif