// Part of dump1090, a Mode S message decoder for RTLSDR devices.
//
// interactive.c: aircraft tracking and interactive display
//
// Copyright (c) 2014,2015 Oliver Jowett <oliver@mutability.co.uk>
//
// This file is free software: you may copy, redistribute and/or modify it  
// under the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 2 of the License, or (at your  
// option) any later version.  
//
// This file is distributed in the hope that it will be useful, but  
// WITHOUT ANY WARRANTY; without even the implied warranty of  
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License  
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// This file incorporates work covered by the following copyright and  
// permission notice:
//
//   Copyright (C) 2012 by Salvatore Sanfilippo <antirez@gmail.com>
//
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are
//   met:
//
//    *  Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//    *  Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "dump1090.h"
#include "interactive.h"
#include <stdio.h>

//
//========================= Interactive mode ===============================


int convert_altitude(int ft)
{
    if (Modes.metric)
        return (ft * 0.3048);
    else
        return ft;
}

int convert_speed(int kts)
{
    if (Modes.metric)
        return (kts * 1.852);
    else
        return kts;
}

//
//=========================================================================
//
// Show the currently captured interactive data on screen.
//
void interactiveShowData(void) {
    struct aircraft *a = Modes.aircrafts;
    static uint64_t next_update;
    uint64_t now = mstime();
	
    int count = 0;
    char progress;
    char spinner[4] = "|/-\\";
    double ELLIPSOID[2] = { 6.378137e6, 8.181919084262149e-02 };
    //uint64_t now_ms = mstime();
   //   double A,E,R;
    // Refresh screen every (MODES_INTERACTIVE_REFRESH_TIME) miliseconde
    if (now < next_update)
        return;

    next_update = now + MODES_INTERACTIVE_REFRESH_TIME;


#if 1
	
	// Sort the linked list
	if(Modes.SortBy == 0)
	QListSortByRange(&a,NULL);  // NULL represent the tail of the linked list
	else if(Modes.SortBy == 1)	
	QListSortByAzimuth(&a,NULL);
	else if(Modes.SortBy == 2)
	QListSortByElvation(&a,NULL);
#endif
    // Attempt to reconsile any ModeA/C with known Mode-S
    // We can't condition on Modes.modeac because ModeA/C could be comming
    // in from a raw input port which we can't turn off.
    //interactiveUpdateAircraftModeS();

    progress = spinner[time(NULL)%4];

#ifndef _WIN32
    printf("\x1b[H\x1b[2J");    // Clear the screen
#else
    cls();
#endif

#if 0
    if (Modes.interactive_rtl1090 == 0) {
        printf (
" Hex    Mode  Sqwk  Flight   Alt    Spd  Hdg    Lat      Long   RSSI  Msgs  Ti%c\n", progress);
    } else {
        printf (
" Hex   Flight   Alt      V/S GS  TT  SSR  G*456^ Msgs    Seen %c\n", progress);
    }
    printf(
"-------------------------------------------------------------------------------\n");
#endif   // Dragonyzl 20161011
 time_t InterCurrentTime;
  struct tm *InterCurrentTime_p;
  time (&InterCurrentTime);
  InterCurrentTime_p = localtime (&InterCurrentTime);	//取得当地时间
  printf ("%d-%02d-%02d,  ", (1900 + InterCurrentTime_p->tm_year),
	  (InterCurrentTime_p->tm_mon + 1), InterCurrentTime_p->tm_mday);
  printf
    ("%02d:%02d:%02d\nSITE(Lat:%10.5f Lon:%10.5f Height:%10.5f)\nRange[%3.2f-->%3.2fkm], Azimuth[%3.2f-->%3.2f degree], Elvatiion[%3.2f-->%3.2f degree ]\n",
     InterCurrentTime_p->tm_hour, InterCurrentTime_p->tm_min, InterCurrentTime_p->tm_sec,
     Modes.fUserLat, Modes.fUserLon, Modes.fUserHeight,
     Modes.Range_min / 1000., Modes.Range_max / 1000.,Modes.Az_Start, Modes.Az_Stop,Modes.El_Start,Modes.El_Stop );
#ifndef ANDROID_PHONE
printf    ("Hex     Speed   Lat        Lon          Alt. Seen%c    Time    Az            El      R(km)   Trk.    Flight  Fd@1GHz \n  ",progress);
  printf  ("---------------------------------------------------------------------------------------------------------------------\n");
#else
printf    ("Hex       Az           El       R(km)    Seen%c  Lat        Lon     Trk.    Flight  Fd@1GHz\n",progress);
  printf    ("------------------------------------------------------------------------------------\n");
#endif
    while(a && (count < Modes.interactive_rows)) {

        if ((now - a->seen) < Modes.interactive_display_ttl)
            {
            int msgs  = a->messages;
            int flags = a->modeACflags;

            if ( (((flags & (MODEAC_MSG_FLAG                             )) == 0                    ) && (msgs > 1  ) )
              || (((flags & (MODEAC_MSG_MODES_HIT | MODEAC_MSG_MODEA_ONLY)) == MODEAC_MSG_MODEA_ONLY) && (msgs > 4  ) ) 
              || (((flags & (MODEAC_MSG_MODES_HIT | MODEAC_MSG_MODEC_OLD )) == 0                    ) && (msgs > 127) ) 
              ) {
                char strSquawk[5] = " ";
                char strFl[7]     = " ";
                char strTt[5]     = " ";
                char strGs[5]     = " ";

                if (trackDataValid(&a->squawk_valid)) {
                    snprintf(strSquawk,5,"%04x", a->squawk);
                }

                if (trackDataValid(&a->speed_valid)) {
                    snprintf (strGs, 5,"%3d", convert_speed(a->speed));
                }

                if (trackDataValid(&a->heading_valid)) {
                    snprintf (strTt, 5,"%03d", a->heading);
                }

                if (msgs > 99999) {
                    msgs = 99999;
                }

                if (Modes.interactive_rtl1090) { // RTL1090 display mode
                    if (trackDataValid(&a->altitude_valid)) {
                        snprintf(strFl,6,"F%03d",((a->altitude+50)/100));
                    }
                    printf("%06x %-8s %-4s         %-3s %-3s %4s        %-6d  %-2.0f\n", 
                           a->addr, a->callsign, strFl, strGs, strTt, strSquawk, msgs, (now - a->seen)/1000.0);

                } else {                         // Dump1090 display mode
                    char strMode[5]               = "    ";
                    char strLat[8]                = " ";
                    char strLon[9]                = " ";
                    double * pSig                 = a->signalLevel;
                    double signalAverage = (pSig[0] + pSig[1] + pSig[2] + pSig[3] + 
                                            pSig[4] + pSig[5] + pSig[6] + pSig[7]) / 8.0; 

                    if ((flags & MODEAC_MSG_FLAG) == 0) {
                        strMode[0] = 'S';
                    } else if (flags & MODEAC_MSG_MODEA_ONLY) {
                        strMode[0] = 'A';
                    }
                    if (flags & MODEAC_MSG_MODEA_HIT) {strMode[2] = 'a';}
                    if (flags & MODEAC_MSG_MODEC_HIT) {strMode[3] = 'c';}

                    if (trackDataValid(&a->position_valid)) {
                        snprintf(strLat, 8,"%7.03f", a->lat);
                        snprintf(strLon, 9,"%8.03f", a->lon);
                    }

                    if (trackDataValid(&a->airground_valid) && a->airground == AG_GROUND) {
                        snprintf(strFl, 7," grnd");
                    } else if (Modes.use_gnss && trackDataValid(&a->altitude_gnss_valid)) {
                        snprintf(strFl, 7, "%5dH", convert_altitude(a->altitude_gnss));
                    } else if (trackDataValid(&a->altitude_valid)) {
                        snprintf(strFl, 7, "%5d ", convert_altitude(a->altitude));
                   //}





#if 0
			printf("%s%06X %-4s  %-4s  %-8s %6s %3s  %3s  %7s %8s %5.1f %5d %2.0f\n",
						(a->addr & MODES_NON_ICAO_ADDRESS) ? "~" : " ", (a->addr & 0xffffff),
						strMode, strSquawk, a->callsign, strFl, strGs, strTt,
						strLat, strLon, 10 * log10(signalAverage), msgs, (now - a->seen)/1000.0);
#endif



                   // if ((a->bFlags & MODES_ACFLAGS_LATLON_VALID)  &&  (a->bFlags & MODES_ACFLAGS_ALTITUDE_VALID )) {
						//  TODO: Three member vars (Range, Azimuth, Elvation) are assigned to the aircraft for 
						//  using the QSortListBy[Range, Azimuth, Elvation];
                        //elevation (Modes.fUserLat, Modes.fUserLon, Modes.fUserHeight, a->lat, a->lon, (float) convert_altitude(a->altitude), ELLIPSOID, &(a->Azimuth), &(a->Elvation), &(a->Range) );
					    float DopplerAt1G = EstimateDopplerAt1G(a->Azimuth,a->Elvation,convert_speed(a->speed)*1000.0 /3600,(float)(a->heading)  );
#if 1

                      if (  (a->Range>Modes.Range_min)
						&& (a->Range<Modes.Range_max)
                        && ((float)convert_altitude(a->altitude)>100.0 )
                        && (a->Elvation > Modes.El_Start )
						&& (a->Elvation<Modes.El_Stop)
						&& (  IsAzInRange(a->Azimuth,Modes.Az_Start,Modes.Az_Stop) )  
						)   
					{
                char strAz[10]     = " ";
                char strEl[11]     = " ";
                char strR[7]     = " ";
                char strTimeSeen[11]     = " ";         //  10, has the limit of time
                char strSeen[5]     = " ";
                 snprintf(strAz, 10, "%10.06f", a->Azimuth);  
				 snprintf(strEl, 11, "%11.06f",a->Elvation);
				 snprintf(strR, 7, "%7.04f", a->Range/1000.);
                 snprintf(strTimeSeen, 11, "%7.03f", (a->seen - Modes.TimeIndex) / 1000.);
                 snprintf(strSeen,4, "%4.02f",  (now - a->seen) / 1000.);

 /* 此处需要打印的信息都需要转换为字符串，因为有些信息不是同时解码出来的，打印到文件中的信息同样需要 */
#ifdef  ANDROID_PHONE
			  printf  (  "%06X  %10s %11s %7s %4s %10s %11s %5s  %-8s %10.1f\n",
				               a->addr, strAz,strEl, strR, strSeen, strLat, strLon, strTt, a->callsign,DopplerAt1G);
#else
			  printf  (  "%06X %5s %10s %11s %7s %4s %10s %10s %11s %7s %5s  %-8s  %10.1f\n",
				               a->addr, strGs, strLat, strLon, strFl, strSeen,strTimeSeen, strAz,strEl, strR,strTt, a->callsign,DopplerAt1G);
#endif // ANDRIOD_PHONE

			   if (Modes.RecordFile && Modes.RecordFlag)
			{
			  int err =  fprintf (Modes.RecordFile,   "%06X %5s %10s %11s %7s %4s %10s %10s %11s %7s %5s   %-8s  %10.1f\n",
				               a->addr, strGs, strLat, strLon, strFl, strSeen,strTimeSeen, strAz,strEl, strR, strTt, a->callsign,DopplerAt1G);
			  fflush (Modes.RecordFile);
			  if (err < 0)
			    printf ("File Recoding Error.\n");
			}

                        }
#endif
					}
                }
                count++;
            }
        }
        a = a->next;
    }
}





//=========================================================================
//
// Show the currently captured interactive data on screen.
//
void myShowData(void) {
    struct aircraft *a = Modes.aircrafts;
    static uint64_t next_update;
    uint64_t now = mstime();
	
    int count = 0;
    char progress;
    char spinner[4] = "|/-\\";
  
    //uint64_t now_ms = mstime();
   //   double A,E,R;
    // Refresh screen every (MODES_INTERACTIVE_REFRESH_TIME) miliseconde
    if (now < next_update)
        return;

    next_update = now + MODES_INTERACTIVE_REFRESH_TIME;


    progress = spinner[time(NULL)%4];

#ifndef _WIN32
    printf("\x1b[H\x1b[2J");    // Clear the screen
#else
    cls();
#endif

    while(a && (count < Modes.interactive_rows)) {

        if ((now - a->seen) < Modes.interactive_display_ttl)
            {
            int msgs  = a->messages;
            int flags = a->modeACflags;

            if ( (((flags & (MODEAC_MSG_FLAG                             )) == 0                    ) && (msgs > 1  ) )
              || (((flags & (MODEAC_MSG_MODES_HIT | MODEAC_MSG_MODEA_ONLY)) == MODEAC_MSG_MODEA_ONLY) && (msgs > 4  ) ) 
              || (((flags & (MODEAC_MSG_MODES_HIT | MODEAC_MSG_MODEC_OLD )) == 0                    ) && (msgs > 127) ) 
              ) {
                char strSquawk[5] = " ";
                char strFl[7]     = " ";
                char strTt[5]     = " ";
                char strGs[5]     = " ";

                if (trackDataValid(&a->squawk_valid)) {
                    snprintf(strSquawk,5,"%04x", a->squawk);
                }

                if (trackDataValid(&a->speed_valid)) {
                    snprintf (strGs, 5,"%3d", convert_speed(a->speed));
                }

                if (trackDataValid(&a->heading_valid)) {
                    snprintf (strTt, 5,"%03d", a->heading);
                }

                if (msgs > 99999) {
                    msgs = 99999;
                }

                if (Modes.interactive_rtl1090) { // RTL1090 display mode
                    if (trackDataValid(&a->altitude_valid)) {
                        snprintf(strFl,6,"F%03d",((a->altitude+50)/100));
                    }
                    printf("%06x %-8s %-4s         %-3s %-3s %4s        %-6d  %-2.0f\n", 
                           a->addr, a->callsign, strFl, strGs, strTt, strSquawk, msgs, (now - a->seen)/1000.0);

                } else {                         // Dump1090 display mode
                    char strMode[5]               = "    ";
                    char strLat[8]                = " ";
                    char strLon[9]                = " ";
                    double * pSig                 = a->signalLevel;
                    double signalAverage = (pSig[0] + pSig[1] + pSig[2] + pSig[3] + 
                                            pSig[4] + pSig[5] + pSig[6] + pSig[7]) / 8.0; 

                    if ((flags & MODEAC_MSG_FLAG) == 0) {
                        strMode[0] = 'S';
                    } else if (flags & MODEAC_MSG_MODEA_ONLY) {
                        strMode[0] = 'A';
                    }
                    if (flags & MODEAC_MSG_MODEA_HIT) {strMode[2] = 'a';}
                    if (flags & MODEAC_MSG_MODEC_HIT) {strMode[3] = 'c';}

                    if (trackDataValid(&a->position_valid)) {
                        snprintf(strLat, 8,"%7.03f", a->lat);
                        snprintf(strLon, 9,"%8.03f", a->lon);
                    }

                    if (trackDataValid(&a->airground_valid) && a->airground == AG_GROUND) {
                        snprintf(strFl, 7," grnd");
                    } else if (Modes.use_gnss && trackDataValid(&a->altitude_gnss_valid)) {
                        snprintf(strFl, 7, "%5dH", convert_altitude(a->altitude_gnss));
                    } else if (trackDataValid(&a->altitude_valid)) {
                        snprintf(strFl, 7, "%5d ", convert_altitude(a->altitude));
                   //}



					    float DopplerAt1G = EstimateDopplerAt1G(a->Azimuth,a->Elvation,convert_speed(a->speed)*1000.0 /3600,(float)(a->heading)  );
#if 1

                      if (  (a->Range>Modes.Range_min)
						&& (a->Range<Modes.Range_max)
                        && ((float)convert_altitude(a->altitude)>100.0 )
                        && (a->Elvation > Modes.El_Start )
						&& (a->Elvation<Modes.El_Stop)
						&& (  IsAzInRange(a->Azimuth,Modes.Az_Start,Modes.Az_Stop) )  
						)   
					{
                char strAz[10]     = " ";
                char strEl[11]     = " ";
                char strR[7]     = " ";
                char strTimeSeen[11]     = " ";         //  10, has the limit of time
                char strSeen[5]     = " ";
                 snprintf(strAz, 10, "%10.06f", a->Azimuth);  
				 snprintf(strEl, 11, "%11.06f",a->Elvation);
				 snprintf(strR, 7, "%7.04f", a->Range/1000.);
                 snprintf(strTimeSeen, 11, "%7.03f", (a->seen - Modes.TimeIndex) / 1000.);
                 snprintf(strSeen,4, "%4.02f",  (now - a->seen) / 1000.);

 /* 此处需要打印的信息都需要转换为字符串，因为有些信息不是同时解码出来的，打印到文件中的信息同样需要 */
#ifdef  ANDROID_PHONE
			  printf  (  "%06X  %10s %11s %7s %4s %10s %11s %5s  %-8s %10.1f\n",
				               a->addr, strAz,strEl, strR, strSeen, strLat, strLon, strTt, a->callsign,DopplerAt1G);
#else
			  printf  (  "%06X %5s %10s %11s %7s %4s %10s %10s %11s %7s %5s  %-8s  %10.1f\n",
				               a->addr, strGs, strLat, strLon, strFl, strSeen,strTimeSeen, strAz,strEl, strR,strTt, a->callsign,DopplerAt1G);
#endif // ANDRIOD_PHONE

			   if (Modes.RecordFile && Modes.RecordFlag)
			{
			  int err =  fprintf (Modes.RecordFile,   "%06X %5s %10s %11s %7s %4s %10s %10s %11s %7s %5s   %-8s  %10.1f\n",
				               a->addr, strGs, strLat, strLon, strFl, strSeen,strTimeSeen, strAz,strEl, strR, strTt, a->callsign,DopplerAt1G);
			  fflush (Modes.RecordFile);
			  if (err < 0)
			    printf ("File Recoding Error.\n");
			}

                        }
#endif
					}
                }
                count++;
            }
        }
        a = a->next;
    }
}



//  The following section is from dragonyzl's dump1090 for sorting and recording
//
//=========================================================================
//

void setRecordFileNamePrefix (void)
  {
    time_t CurrentTime;
    struct tm *CurrentTime_p;


    char RecordFileName[1024];
    strcpy (RecordFileName, Modes.pathname);
    strcat (RecordFileName, "/Record_");
    char TempString[1024];
    strcpy (TempString, "");
    time (&CurrentTime);
    CurrentTime_p = localtime (&CurrentTime);	//取得当地时间

    printf ("%d-%d-%d,  ", (1900 + CurrentTime_p->tm_year),
	    (CurrentTime_p->tm_mon + 1), CurrentTime_p->tm_mday);
    sprintf (TempString, "%d_%d_%d", (1900 + CurrentTime_p->tm_year),
	     (CurrentTime_p->tm_mon + 1), CurrentTime_p->tm_mday);
    strcat (RecordFileName, TempString);

    printf ("%d:%d:%d\n", CurrentTime_p->tm_hour, CurrentTime_p->tm_min,
	    CurrentTime_p->tm_sec);
    sprintf (TempString, "_%d_%d_%d", CurrentTime_p->tm_hour,
	     CurrentTime_p->tm_min, CurrentTime_p->tm_sec);
    strcat (RecordFileName, TempString);

    struct timeval tv;
    gettimeofday (&tv, NULL);
    sprintf (TempString, "_%06ld", tv.tv_usec);


    strcat (RecordFileName, TempString);
    strcat (RecordFileName, ".dat");

    if (Modes.RecordFlag)
      {
	Modes.RecordFile = fopen (RecordFileName, "w");
	if (Modes.RecordFile)
	  {
	    fprintf (Modes.RecordFile, "%s at the Lat:%10.7f,  lon:%10.7f,  alt:%10.7f  \n", RecordFileName,Modes.fUserLat, Modes.fUserLon, Modes.fUserHeight);
	   // printf ("%s at the Lat:%10.7f,  lon:%10.7f,  alt:%10.7f  \n", RecordFileName,Modes.fUserLat, Modes.fUserLon, Modes.fUserHeight);
		//getchar();
	    fprintf (Modes.RecordFile,"Hex     Speed   Lat        Lon          Alt. Seen    Time    Az            El      R(km)  Trk.   Flight  Fd@1GHz\n");
		  }


	else
	  {
	    printf
	      ("Record File open Error,Data Can not be recorded, Just Display the plane\n");
	    Modes.RecordFlag = 0;
	  }

      }
  }








// The following three functions refered from http://blog.csdn.net/pinkrobin/article/details/5456094, Thanks 
void QListSortByRange(struct aircraft **head, struct aircraft *end) {  
    struct aircraft *right;  
    struct aircraft **left_walk, **right_walk;  
    struct aircraft *pivot, *old;  
    int count, left_count, right_count;  
    if (*head == end)  
        return;  
    do {  
        pivot = *head;  
        left_walk = head;  
        right_walk = &right;  
        left_count = right_count = 0;  
        //取第一个节点作为比较的基准，小于基准的在左面的子链表中，  
        //大于基准的在右边的子链表中  
        for (old = (*head)->next; old != end; old = old->next) {  
            if (old->Range < pivot->Range) {   //小于基准,加入到左面的子链表,继续比较  
                ++left_count;  
                *left_walk = old;            //把该节点加入到左边的链表中，  
                left_walk = &(old->next);  
            } else {                         //大于基准,加入到右边的子链表，继续比较  
                ++right_count;  
                *right_walk = old;             
                right_walk = &(old->next);  
            }  
        }  
        //合并链表  
        *right_walk = end;       //结束右链表  
        *left_walk = pivot;      //把基准置于正确的位置上  
        pivot->next = right;     //把链表合并  
        //对较小的子链表进行快排序，较大的子链表进行迭代排序。  
        if(left_walk > right_walk) {  
            QListSortByRange(&(pivot->next), end);  
            end = pivot;  
            count = left_count;  
        } else {  
            QListSortByRange(head, pivot);  
            head = &(pivot->next);  
            count = right_count;  
        }  
    } while (count > 1);   
}  



void QListSortByAzimuth(struct aircraft **head, struct aircraft *end) {  
    struct aircraft *right;  
    struct aircraft **left_walk, **right_walk;  
    struct aircraft *pivot, *old;  
    int count, left_count, right_count;  
    if (*head == end)  
        return;  
    do {  
        pivot = *head;  
        left_walk = head;  
        right_walk = &right;  
        left_count = right_count = 0;  
        //取第一个节点作为比较的基准，小于基准的在左面的子链表中，  
        //大于基准的在右边的子链表中  
        for (old = (*head)->next; old != end; old = old->next) {  
            if (old->Azimuth < pivot->Azimuth) {   //小于基准,加入到左面的子链表,继续比较  
                ++left_count;  
                *left_walk = old;            //把该节点加入到左边的链表中，  
                left_walk = &(old->next);  
            } else {                         //大于基准,加入到右边的子链表，继续比较  
                ++right_count;  
                *right_walk = old;             
                right_walk = &(old->next);  
            }  
        }  
        //合并链表  
        *right_walk = end;       //结束右链表  
        *left_walk = pivot;      //把基准置于正确的位置上  
        pivot->next = right;     //把链表合并  
        //对较小的子链表进行快排序，较大的子链表进行迭代排序。  
        if(left_walk > right_walk) {  
            QListSortByAzimuth(&(pivot->next), end);  
            end = pivot;  
            count = left_count;  
        } else {  
            QListSortByAzimuth(head, pivot);  
            head = &(pivot->next);  
            count = right_count;  
        }  
    } while (count > 1);   
}  




void QListSortByElvation(struct aircraft **head, struct aircraft *end) {  
    struct aircraft *right;  
    struct aircraft **left_walk, **right_walk;  
    struct aircraft *pivot, *old;  
    int count, left_count, right_count;  
    if (*head == end)  
        return;  
    do {  
        pivot = *head;  
        left_walk = head;  
        right_walk = &right;  
        left_count = right_count = 0;  
        //取第一个节点作为比较的基准，小于基准的在左面的子链表中，  
        //大于基准的在右边的子链表中  
        for (old = (*head)->next; old != end; old = old->next) {  
            if (old->Elvation < pivot->Elvation) {   //小于基准,加入到左面的子链表,继续比较  
                ++left_count;  
                *left_walk = old;            //把该节点加入到左边的链表中，  
                left_walk = &(old->next);  
            } else {                         //大于基准,加入到右边的子链表，继续比较  
                ++right_count;  
                *right_walk = old;             
                right_walk = &(old->next);  
            }  
        }  
        //合并链表  
        *right_walk = end;       //结束右链表  
        *left_walk = pivot;      //把基准置于正确的位置上  
        pivot->next = right;     //把链表合并  
        //对较小的子链表进行快排序，较大的子链表进行迭代排序。  
        if(left_walk > right_walk) {  
            QListSortByElvation(&(pivot->next), end);  
            end = pivot;  
            count = left_count;  
        } else {  
            QListSortByElvation(head, pivot);  
            head = &(pivot->next);  
            count = right_count;  
        }  
    } while (count > 1);   
}  

