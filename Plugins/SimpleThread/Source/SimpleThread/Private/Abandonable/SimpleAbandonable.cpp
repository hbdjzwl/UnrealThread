#include "Abandonable/SimpleAbandonable.h"

FSimpleAbandonable::FSimpleAbandonable(const FSimpleDelegate& InThreadDelegate)
	:ThreadDelegate(InThreadDelegate)
{
}

void FSimpleAbandonable::DoWork()
{
	ThreadDelegate.ExecuteIfBound(); //Ö´ÐÐ´úÀí
}