
/* #line 1 "/root/pgtalt/command.rl" */
/*
 * Parses LX200 protocol.
 */
#include <string.h>
#include <stdio.h>
#include "mount.h"
#include "misc.h"
#include <math.h>
#include "taki.h"
#define MOVE printf("move ");
#define ADD_DIGIT(var,digit) var=var*10+digit-'0';
#define APPEND strcat(buffcom,tmessage);//printf("%s\r\n", buffcom);
char buffcom [200];
char tmessage[50];
const int month_days[] = {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
extern c_star st_now, st_target, st_current;
extern  char one;
struct _telescope_
{
    long dec_target,ra_target;
    long alt_target,az_target;
    long current_dec,current_ra;
    long lat,longitude;
    char day,month,year,dayofyear;
}
mount;
extern mount_t *mount_test;
void lxprintde(long x)

{
    char c='+';
    if (x<0)
    {
        x=-x;
        c= '-';
    }

    long gra=x/3600;
    long temp=(x %3600);
    int min=temp/60;
    int sec=temp%60;
    sprintf(tmessage,"%c%02d%c%02d'%02d#",c,gra,223,min,sec);
    APPEND
};

void lxprintra(long x)
{
    long gra=x/3600;
    long temp=(x %3600);
    int min=temp/60;
    int sec=temp%60;
    sprintf(tmessage,"%02d:%02d:%02d#",gra,min,sec);
    APPEND
};
void lxprintdate(void)
{
    printf ("%02d/%02d/%02d#",mount.month,mount.day,mount.year);
}
void lxprintsite(void)
{
    printf("Site Name#");
};
void move(char fc)
{ mount_test->altmotor->slewing= mount_test->azmotor->slewing=FALSE;
 mount_test->is_tracking=FALSE;
int srate=mount_test->srate;
    switch (fc)
    {
    case 'n':

       //  mount_test->altmotor->targetspeed=SID_RATE*mount_test->rate[srate]*SEC_TO_RAD;
        settargetspeed(mount_test->altmotor,SID_RATE*mount_test->rate[srate]*SEC_TO_RAD);
        break;
    case 's':
        settargetspeed(mount_test->altmotor,-SID_RATE*mount_test->rate[srate]*SEC_TO_RAD);
     //   mount_test->altmotor->targetspeed=-SID_RATE*mount_test->rate[srate]*SEC_TO_RAD;
        break;
    case 'w':
    settargetspeed(mount_test->azmotor,SID_RATE*mount_test->rate[srate]*SEC_TO_RAD);
       // mount_test->azmotor->targetspeed= SID_RATE*mount_test->rate[srate]*SEC_TO_RAD;
        break;
    case 'e':
        settargetspeed(mount_test->azmotor,-SID_RATE*mount_test->rate[srate]*SEC_TO_RAD);
       // mount_test->azmotor->targetspeed=-SID_RATE*mount_test->rate[srate]*SEC_TO_RAD;
        break;
    };
};


void ltime(void)
{
    long pj =(long)1;// (timer0GetOverflowCount ()  /30.518);
    lxprintra(pj);
    //;rprintf("1");
}

void set_cmd_exe(char cmd,long date)
{
    switch (cmd)
    {
    case 'r':
        mount.ra_target=date;


        break;
    case 'd':
        mount.dec_target=date ;
        break;
    case 'a':
        mount.alt_target=date;
        break;
    case 'z':
        mount.az_target=date ;
        break;
    case 't':
        mount.lat=date ;
        mount_test->lat=date;
        break;
    case 'g':
        mount.longitude=date ;
          mount_test->longitude;
        break;
    case 'L' :
        //timer0SetOverflowCount((long) (30.518 *date));
        break;
    case 'S':
        break;

    }

    //sprintf(tmessage,"1");APPEND
}
void set_date( int day,int month,int year)
{

    mount.month=month;
    mount.day=day;
    mount.year=year;
    mount.dayofyear=day+month_days[month-1];
    if  ((month>2)&&(year%4==0)) mount.dayofyear++;

}
void sync_all(void)
{int temp;
   // mount_test->track=FALSE;
   mount_test->altmotor->slewing= mount_test->azmotor->slewing=FALSE;
   sync_ra_dec(mount_test,mount.ra_target*15.0*SEC_TO_RAD,mount.dec_target*SEC_TO_RAD);
    sprintf(tmessage,"sync#");
    APPEND

};
void stop(char fc)
{
  mount_test->altmotor->slewing= mount_test->azmotor->slewing=FALSE;
    switch (fc)
    {
    case 'n': case 's':
        mount_test->altmotor->targetspeed=0.0;
       do {usleep( 10000);} while (fabs(mount_test->altmotor->current_speed)>0.0);
        break;


    case 'w':   case 'e':
        mount_test->azmotor->targetspeed=0.0;
            do {usleep( 10000);} while (fabs(mount_test->azmotor->current_speed)>0.0);

        break;

    default:
        mount_test->altmotor->targetspeed=0.0;

        break;
    };
one=FALSE;mount_test->is_tracking=TRUE;

}

void rate(char fc)
 {
    switch (fc)
    {
        case 'C':mount_test->srate=1;
        break;
        case 'G':mount_test->srate=0;
        break;
        case 'M':mount_test->srate=2;
        break;
        case 'S':mount_test->srate=3;
        break;


    };

}

//----------------------------------------------------------------------------------------
long command( char *str )
{
    char *p = str, *pe = str + strlen( str );
    int cs;
    char stcmd;
    long deg=0;
    int min=0;
    int sec=0;

    // deg=min=sec=0;
    int neg = 1;
    tmessage[0]=0;
    buffcom[0]=0;

    
/* #line 215 "command.c" */
static const char _command_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 16, 1, 17, 1, 18, 1, 
	19, 1, 20, 1, 23, 1, 24, 2, 
	1, 21, 2, 2, 22
};

static const char _command_key_offsets[] = {
	0, 0, 6, 7, 8, 20, 21, 22, 
	23, 24, 25, 26, 27, 28, 29, 34, 
	35, 36, 41, 42, 46, 47, 57, 58, 
	60, 62, 63, 65, 67, 68, 70, 72, 
	73, 78, 80, 82, 83, 85, 87, 89, 
	91, 92, 94, 96, 103, 107, 109, 111, 
	111, 113, 115, 117, 118, 119, 120, 122
};

static const char _command_trans_keys[] = {
	67, 71, 77, 81, 82, 83, 77, 35, 
	65, 67, 68, 76, 77, 82, 83, 90, 
	100, 103, 114, 116, 35, 35, 35, 35, 
	35, 35, 35, 35, 35, 83, 101, 110, 
	115, 119, 35, 35, 35, 101, 110, 115, 
	119, 35, 67, 71, 77, 83, 35, 67, 
	76, 83, 97, 100, 103, 114, 116, 119, 
	122, 32, 48, 57, 48, 57, 47, 48, 
	57, 48, 57, 47, 48, 57, 48, 57, 
	35, 32, 9, 13, 48, 50, 48, 50, 
	48, 57, 58, 48, 53, 48, 57, 46, 
	58, 48, 57, 35, 48, 53, 48, 57, 
	32, 43, 45, 9, 13, 48, 57, 43, 
	45, 48, 57, 48, 57, 48, 57, 48, 
	53, 48, 57, 35, 58, 32, 51, 35, 
	35, 58, 58, 0
};

static const char _command_single_lengths[] = {
	0, 6, 1, 1, 12, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 5, 1, 
	1, 5, 1, 4, 1, 10, 1, 0, 
	0, 1, 0, 0, 1, 0, 0, 1, 
	1, 0, 0, 1, 0, 0, 2, 0, 
	1, 0, 0, 3, 2, 0, 0, 0, 
	0, 0, 2, 1, 1, 1, 2, 1
};

static const char _command_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 0, 1, 1, 0, 1, 1, 0, 
	2, 1, 1, 0, 1, 1, 0, 1, 
	0, 1, 1, 2, 1, 1, 1, 0, 
	1, 1, 0, 0, 0, 0, 0, 0
};

static const char _command_index_offsets[] = {
	0, 0, 7, 9, 11, 24, 26, 28, 
	30, 32, 34, 36, 38, 40, 42, 48, 
	50, 52, 58, 60, 65, 67, 78, 80, 
	82, 84, 86, 88, 90, 92, 94, 96, 
	98, 102, 104, 106, 108, 110, 112, 115, 
	117, 119, 121, 123, 129, 133, 135, 137, 
	138, 140, 142, 145, 147, 149, 151, 154
};

static const char _command_indicies[] = {
	0, 2, 3, 4, 5, 6, 1, 7, 
	1, 8, 1, 9, 10, 9, 11, 12, 
	13, 11, 9, 14, 15, 16, 17, 1, 
	18, 1, 19, 1, 20, 1, 21, 1, 
	22, 1, 23, 1, 24, 1, 25, 1, 
	26, 1, 27, 28, 28, 28, 28, 1, 
	29, 1, 30, 1, 31, 32, 32, 32, 
	32, 1, 31, 1, 33, 33, 33, 33, 
	1, 34, 1, 35, 36, 36, 37, 37, 
	37, 36, 37, 38, 37, 1, 39, 1, 
	40, 1, 41, 1, 42, 1, 43, 1, 
	44, 1, 45, 1, 46, 1, 47, 1, 
	48, 1, 49, 49, 50, 1, 50, 1, 
	51, 1, 52, 1, 53, 1, 54, 1, 
	55, 56, 1, 57, 1, 58, 1, 59, 
	1, 60, 1, 61, 62, 63, 61, 64, 
	1, 62, 63, 64, 1, 65, 1, 66, 
	1, 67, 68, 1, 69, 1, 58, 56, 
	1, 70, 1, 71, 1, 72, 1, 73, 
	74, 1, 74, 1, 0
};

static const char _command_trans_targs[] = {
	2, 0, 4, 14, 17, 19, 21, 3, 
	55, 5, 6, 7, 8, 9, 10, 11, 
	12, 13, 55, 55, 55, 55, 55, 55, 
	55, 55, 55, 15, 16, 55, 55, 55, 
	18, 20, 55, 22, 32, 43, 51, 23, 
	24, 25, 26, 27, 28, 29, 30, 31, 
	55, 33, 34, 35, 36, 37, 38, 39, 
	41, 40, 55, 42, 40, 44, 45, 45, 
	45, 46, 47, 48, 49, 50, 52, 53, 
	55, 55, 1
};

static const char _command_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	35, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 19, 25, 39, 27, 17, 23, 
	33, 21, 31, 0, 43, 11, 9, 13, 
	43, 43, 15, 0, 43, 43, 0, 0, 
	3, 3, 0, 5, 5, 0, 1, 1, 
	45, 0, 1, 1, 0, 3, 47, 0, 
	0, 37, 41, 5, 50, 0, 0, 7, 
	1, 1, 1, 0, 3, 47, 0, 0, 
	29, 0, 0
};

static const int command_start = 54;
static const int command_first_final = 54;
static const int command_error = 0;

static const int command_en_main = 54;


/* #line 214 "/root/pgtalt/command.rl" */





    
/* #line 348 "command.c" */
	{
	cs = command_start;
	}

/* #line 353 "command.c" */
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _command_trans_keys + _command_key_offsets[cs];
	_trans = _command_index_offsets[cs];

	_klen = _command_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _command_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _command_indicies[_trans];
	cs = _command_trans_targs[_trans];

	if ( _command_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _command_actions + _command_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
/* #line 221 "/root/pgtalt/command.rl" */
	{ADD_DIGIT(deg,(*p)); }
	break;
	case 1:
/* #line 222 "/root/pgtalt/command.rl" */
	{ADD_DIGIT(min,(*p)); }
	break;
	case 2:
/* #line 223 "/root/pgtalt/command.rl" */
	{ADD_DIGIT(sec,(*p)); }
	break;
	case 3:
/* #line 226 "/root/pgtalt/command.rl" */
	{ neg=-1;}
	break;
	case 4:
/* #line 227 "/root/pgtalt/command.rl" */
	{move(stcmd);}
	break;
	case 5:
/* #line 228 "/root/pgtalt/command.rl" */
	{goto_ra_dec(mount_test,mount.ra_target*15.0*SEC_TO_RAD,mount.dec_target*SEC_TO_RAD); sprintf(tmessage,"0");APPEND;
         //mount_test->track=TRUE;
         mount_test->altmotor->slewing= mount_test->azmotor->slewing=TRUE;
         }
	break;
	case 6:
/* #line 232 "/root/pgtalt/command.rl" */
	{stop(stcmd);}
	break;
	case 7:
/* #line 234 "/root/pgtalt/command.rl" */
	{rate(stcmd); }
	break;
	case 8:
/* #line 236 "/root/pgtalt/command.rl" */
	{ lxprintra1(tmessage, st_current.ra);; APPEND;}
	break;
	case 9:
/* #line 237 "/root/pgtalt/command.rl" */
	{lxprintde1(tmessage, st_current.dec);; APPEND;}
	break;
	case 10:
/* #line 241 "/root/pgtalt/command.rl" */
	{ lxprintra(mount.ra_target);}
	break;
	case 11:
/* #line 242 "/root/pgtalt/command.rl" */
	{lxprintde( mount.dec_target);}
	break;
	case 12:
/* #line 243 "/root/pgtalt/command.rl" */
	{lxprintdate();}
	break;
	case 13:
/* #line 244 "/root/pgtalt/command.rl" */
	{ lxprintsite();}
	break;
	case 14:
/* #line 245 "/root/pgtalt/command.rl" */
	{;}
	break;
	case 15:
/* #line 246 "/root/pgtalt/command.rl" */
	{sprintf(tmessage,"-004ß12#");}
	break;
	case 16:
/* #line 247 "/root/pgtalt/command.rl" */
	{sprintf(tmessage,"+36ß53#");}
	break;
	case 17:
/* #line 251 "/root/pgtalt/command.rl" */
	{sync_all();}
	break;
	case 18:
/* #line 255 "/root/pgtalt/command.rl" */
	{deg+=((*p)-'0')*6;}
	break;
	case 19:
/* #line 256 "/root/pgtalt/command.rl" */
	{ ltime();}
	break;
	case 20:
/* #line 258 "/root/pgtalt/command.rl" */
	{
            set_cmd_exe(stcmd,(neg*(deg )));
            sprintf(tmessage,"1");APPEND;
            deg=sec=min=0;
        }
	break;
	case 21:
/* #line 263 "/root/pgtalt/command.rl" */
	{deg=deg*3600+min*60;}
	break;
	case 22:
/* #line 264 "/root/pgtalt/command.rl" */
	{deg+=sec;}
	break;
	case 23:
/* #line 265 "/root/pgtalt/command.rl" */
	{stcmd=(*p);}
	break;
	case 24:
/* #line 266 "/root/pgtalt/command.rl" */
	{set_date(sec,min,deg);}
	break;
/* #line 534 "command.c" */
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

/* #line 288 "/root/pgtalt/command.rl" */


//---------------------------------------------------------------------------------------------------------------------
    if ( cs < command_first_final )
        //	fprintf( stderr, "LX command:  error\n" );

        return  neg;
};



