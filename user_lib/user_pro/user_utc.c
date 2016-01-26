

int hyear = 1970;
int  hmonth = 1;
int  hday = 1;
int  htime = 0;
int  hminute = 0;
int  hsecond = 0;


//long time_second = 33333;
int mon[12] = { 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

//argv[] = { yearl, yearh, month, day, time, minute, sencond }
int utc_to_ctime(unsigned long sencond, unsigned char *argv)
{   
		int  tmp = hyear;
		int  tday = 0;
		int  cday = sencond / 60 / 60 / 24 + 1;
		int  lap = 0;

    while(1)
    {
		if( (tmp % 4 == 0 && tmp % 100 != 0) || (tmp % 400 == 0) )
		{
			tday = tday + 366;
			if(tday > cday)
			{
				cday = cday - (tday - 366);
				lap = 1;
				break;
			}
		} else {
			tday = tday + 365;
			if(tday > cday)
			{
				cday = cday - (tday - 365);
				lap = 0;
				break;
			}
		}

		tmp = tmp + 1;
    } 

	argv[0] = (tmp & 0x000000ff);
	argv[1] = ((tmp >> 8) & 0x000000ff);
	mon[1] = mon[1] + lap;

	if(cday <= mon[0])
	{
		argv[2] = 1;
		argv[3] = cday;
	} else if(cday > mon[0] && cday <= mon[1]) {
		argv[2] = 2;
	  argv[3] = cday - mon[0];
	} else if(cday > mon[1] && cday <= mon[2]) {
		argv[2] = 3;
		argv[3] = cday - mon[1];
	} else if(cday > mon[2] && cday <= mon[3]) {
		argv[2] = 4;
		argv[3] = cday - mon[2];
	} else if(cday > mon[3] && cday <= mon[4]) {
		argv[2] = 5;
		argv[3] = cday - mon[3];
	} else if(cday > mon[4] && cday <= mon[5]) {
		argv[2] = 6;
		argv[3] = cday - mon[4];
	} else if(cday > mon[5] && cday <= mon[6]) {
		argv[2] = 7;
		argv[3] = cday - mon[5];
	} else if(cday > mon[6] && cday <= mon[7]) {
		argv[2] = 8;
		argv[3] = cday - mon[6];
	} else if(cday > mon[7] && cday <= mon[8]) {
		argv[2] = 9;
		argv[3] = cday - mon[7];
	} else if(cday > mon[8] && cday <= mon[9]) {
		argv[2] = 10;
		argv[3] = cday - mon[8];
	} else if(cday > mon[9] && cday <= mon[10]) {
		argv[2] = 11;
		argv[3] = cday - mon[9];
	} else if(cday > mon[10] && cday <= mon[11]){
		argv[2] = 12;
		argv[3] = cday - mon[10];
	}
	
	argv[4] = ((sencond / 60 / 60 ) + 8) % 24;
	argv[5] = (sencond / 60) % 60;
	argv[6] = sencond % 60;

	return 0;
}

