

int  hyear = 1970;
int  hmonth = 1;
int  hday = 1;
int  htime = 0;
int  hminute = 0;
int  hsecond = 0;


//long time_second = 33333;
int mon[12] = { 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

int week(int year, int mont, int day)
{
	int day_code, s=0;
	int wk;
	int i;

	day_code=(year + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400 ) % 7;

	for(i = 0 ; i < mont - 1 ; i++)
		s = mon[i];

	s = s + day;

	if( year % 4 == 0)
		s = s + 1;

	wk = ( s + day_code - 1) % 7;

	return wk;
}

//argv[] = { 4:yearl, 5:yearh, 6:sencond, 7:minute, 8:time, 9:week, 10:day, 11:month }
int utc_to_ctime(unsigned long sencond, unsigned char *argv)
{   
		int  tmp = hyear;
	
		int  year;
		int  month;
		int  day;
		int  wk;
	  int  time;
	  int  minute;
	  int  second;
	
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
	
	year = tmp;  // year
		
	mon[1] = mon[1] + lap;

	if(cday <= mon[0])
	{
		month = 1;
		day = cday;
	} else if(cday > mon[0] && cday <= mon[1]) {
		month = 2;
	  day = cday - mon[0];
	} else if(cday > mon[1] && cday <= mon[2]) {
		month = 3;
		day = cday - mon[1];
	} else if(cday > mon[2] && cday <= mon[3]) {
		month = 4;
		day = cday - mon[2];
	} else if(cday > mon[3] && cday <= mon[4]) {
		month = 5;
		day = cday - mon[3];
	} else if(cday > mon[4] && cday <= mon[5]) {
		month = 6;
		day = cday - mon[4];
	} else if(cday > mon[5] && cday <= mon[6]) {
		month = 7;
		day = cday - mon[5];
	} else if(cday > mon[6] && cday <= mon[7]) {
		month = 8;
		day = cday - mon[6];
	} else if(cday > mon[7] && cday <= mon[8]) {
		month = 9;
		day = cday - mon[7];
	} else if(cday > mon[8] && cday <= mon[9]) {
		month = 10;
		day = cday - mon[8];
	} else if(cday > mon[9] && cday <= mon[10]) {
		month = 11;
		day = cday - mon[9];
	} else if(cday > mon[10] && cday <= mon[11]){
		month = 12;
		day = cday - mon[10];
	}
	
	time = ((sencond / 60 / 60 ) + 8) % 24;
	minute = (sencond / 60) % 60;
	second = sencond % 60;
	wk = week(year, month, day);
	
	//argv[] = { 4:yearl, 5:yearh, 6:sencond, 7:minute, 8:time, 9:week, 10:day, 11:month }
	argv[4] = (year & 0x000000ff);
	argv[5] = ((year >> 8) & 0x000000ff);
	argv[6] = second;
	argv[7] = minute;
	argv[8] = time;
	argv[9] = wk - 1;
	argv[10] = day;
	argv[11] = month;

	return 0;
}

