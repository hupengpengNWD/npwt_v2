

unsigned long T1_TimeGet()
{
	return  Timeone_sum ;
}

void TimeOutSet(TIMER	*timer,	unsigned long 	timeout)
{
	timer->RecTickVal = T1_TimeGet();
	timer->TimeOutVal = T1_TimeGet() + timeout;
	timer->IsTimeOut = FALSE;
}	

unsigned char IsTimeOut(	TIMER 	*timer)
{
	if(FALSE==timer->IsTimeOut)
	{
		if((long)(T1_TimeGet() - timer->TimeOutVal) >= 0)
		{
			timer->IsTimeOut = TRUE;
		}
	}		

	return timer->IsTimeOut;
}

