/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2014  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
     WWWWWWWWWWWWWWWWWWW         | This file is part of White Cat.
    WWWWWWWWWWWWWWWWWCWWWW       |
   WWWWWWWWWWWWWWWWW tWWWWW      | White Cat is free software: you can redistribute it and/or modify
  WWWW   WWWWWWWWWW  tWWWWWW     | it under the terms of the GNU General Public License as published by
 WWWWWt              tWWWWWWa    | the Free Software Foundation, either version 2 of the License, or
 WWWWWW               WWWWWWW    | (at your option) any later version.
WWWWWWWW              WWWWWWW    |
WWWWWWWW               WWWWWWW   | White Cat is distributed in the hope that it will be useful,
WWWWWWW               WWWWWWWW   | but WITHOUT ANY WARRANTY; without even the implied warranty of
WWWWWWW      CWWW    W WWWWWWW   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
WWWWWWW            aW  WWWWWWW   | GNU General Public License for more details.
WWWWWWWW           C  WWWWWWWW   |
 WWWWWWWW            CWWWWWWW    | You should have received a copy of the GNU General Public License
 WWWWWWWWW          WWWWWWWWW    | along with White Cat.  If not, see <http://www.gnu.org/licenses/>.
  WWWWWWWWWWC    CWWWWWWWWWW     |
   WWWWWWWWWWWWWWWWWWWWWWWW      |
    WWWWWWWWWWWWWWWWWWWWWW       |
      WWWWWWWWWWWWWWWWWWa        |
        WWWWWWWWWWWWWWW          |
           WWWWWWWWt             |
                                 |
---------------------------------------------------------------------------------------------------------------*/

/**

* \file saves_export_import.cpp
* \brief {import and export the show in ASCII}
* \author Christoph Guillermet
* \version {0.8.6.3}
* \date {12/02/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Fonctions d'import export ASCII pour charger ou envoyer une conduite vers ou depuis une console ou un autre programme
*
*   Import and export the show in ASCII from or to a console or a different software
*
**/

#include "wc_tus.h"

int do_reset_all_listproj()
{
for (int r=0;r<514;r++)
{
 strcpy(descriptif_projecteurs[r],"");
 descriptif_projecteurs[r][0]='\0';
}
 return(0);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////ASCII///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// [ascii interop] Eos/Cobalt ecrivent les mots-cles en TitleCase (Cue, Up, Down, Chan) ; WhiteCat
// les ecrivait en MAJUSCULES. Comparaison de prefixe insensible a la casse pour lire les deux.
static int wc_ci_prefix(const char* s, const char* p){
  for(; *p; ++s, ++p){
    char a=*s, b=*p;
    if(a>='A'&&a<='Z') a+=32;
    if(b>='A'&&b<='Z') b+=32;
    if(a!=b) return 0;
  }
  return 1;
}
static int wc_hexval(char c){
  if(c>='0'&&c<='9') return c-'0';
  if(c>='a'&&c<='f') return c-'a'+10;
  if(c>='A'&&c<='F') return c-'A'+10;
  return -1;
}
// [ascii interop] parse un token niveau "<chan><sep>H<hex>" (sep = @ Eos, ou / ancien WhiteCat).
// La LARGEUR de l'hexa donne la resolution : 2 chiffres = 8 bit (x257), 4 chiffres = 16 bit direct
// (ex. Eos : 2@Hff = 8 bit plein ; 1@Hffff / 5@H30a4 = 16 bit). Retourne 0 si non parsable.
static int wc_parse_chan_level(const char* tok, int* out_chan, int* out_lvl16){
  int c=0; const char* p=tok;
  if(*p<'0'||*p>'9') return 0;
  while(*p>='0'&&*p<='9'){ c=c*10+(*p-'0'); ++p; }
  while(*p && *p!='@' && *p!='/') ++p;      // separateur @ (Eos) ou / (WhiteCat)
  if(*p!='@' && *p!='/') return 0;
  ++p;
  if(*p!='H' && *p!='h') return 0;          // on ne gere que l'hexa (@H)
  ++p;
  int hl=0, v=0, d;
  while((d=wc_hexval(p[hl]))>=0){ v=v*16+d; ++hl; }
  if(hl==0) return 0;
  int lvl16 = (hl>=3) ? v : (int)wc::dmx8_to_lvl((unsigned char)v); // >=3 chiffres -> 16 bit direct
  if(lvl16>65535) lvl16=65535;
  *out_chan=c; *out_lvl16=lvl16; return 1;
}

int do_ASCII_import()
{
index_is_saving=1;
sprintf(rep,"%s/import_export/ascii",mondirectory);
chdir(rep);


////////////////////////////////////////////////////////////////////////////////

	char line [512];
//sab 02/03/2014 unused var	int n=0;
	int cue=0;
	float down=0.0;
	float up=0.0;
	int chan=0;
	int level=0;//garder en int sinon la lecture ne se fait pas !
	float autogotime=0.0;
    int dimmer=0;
//sab 02/03/2014 unused var	bool alarm_cue=0; bool alarm_patch_chan=0; bool alarm_patch_dim=0;
	char *temp;
//sab 02/03/2014 unused var	char *string_text;
	int sub=0;
	int ok= -1;
	int flagcue= -1;
	int flagpatch= -1;
	int flagsub=-1;
	int first_cue_from_import=0;
	// [master import Congo] $MASTPAGEITEM <page> <item> -> dock WCat (fader = item-1, dock = page-1)
	int flagmaster=-1;
	int master_fader=-1;
	int master_dock=-1;



	FILE *f=NULL;


    if ((f=fopen(importfile_name, "rt"))== NULL)
    {
      sprintf(string_save_load_report[0],"Error opening %s",importfile_name); b_report_error[0]=1;

    }
    else
    {
	do {
		if (fgets(line,512,f)!=NULL)
		{
				// [ascii interop] Eos INDENTE les lignes de cue (3 espaces : "   Up 10", "   Chan …").
				// On retire le blanc de tete pour que les tests de mot-cle et les offsets (line+N)
				// marchent pour Eos ET WhiteCat (qui, lui, n'indente pas -> no-op).
				{ char* _s=line; while(*_s==' '||*_s=='\t') _s++;
				  if(_s!=line){ memmove(line,_s,strlen(_s)+1); } }
				// [ascii interop] "CUE 1.5" (WhiteCat) ou "Cue 7 1" (Eos : num + cuelist). Le nombre
				// apres le mot-cle -> memoire = num x10. On efface la memoire avant de la remplir
				// (import propre, et compat avec le remplissage partiel Chan/$$ChanMove).
				if (wc_ci_prefix(line, "CUE") && (line[3]==' '||line[3]=='\t'))
					{
						float tmpcueval= strtof(line+3,NULL);
						cue = (int)(tmpcueval*10.0f + 0.5f);
						if(cue>=0 && cue<10000)
						{
						MemoiresExistantes[cue]=1;
						for(int _cc=0;_cc<514;_cc++){ Memoires[cue][_cc]=0; }
	                    if(first_cue_from_import==0)
	                    {first_cue_from_import=cue;}
						flagcue=1;flagsub=-1;flagpatch=-1;flagmaster=-1;
						}
                    }

        	    if(flagcue==1)//cues
			    {

                 // [ascii interop] "Text …" (WhiteCat/USITT) ou "$$Text …" (Eos) -> label memoire
                 if(wc_ci_prefix(line,"TEXT"))
                 {
                 for (int p=0; p<24;p++)
                 {
                 descriptif_memoires[cue][p]=line[p+5];
                 }
                 descriptif_memoires[cue][24]='\0';
                 }
                 else if(wc_ci_prefix(line,"$$TEXT"))
                 {
                 for (int p=0; p<24;p++)
                 {
                 descriptif_memoires[cue][p]=line[p+7];
                 }
                 descriptif_memoires[cue][24]='\0';
                 }

                 ////0=DIN  1=IN 2=DOUT 3=OUT
                //sprintf(header_export,"Stage: d:%.1f  OUT: %.1f  | Memory: d:%.1f  IN: %.1f", Times_Memoires[m][2], Times_Memoires[m][3],Times_Memoires[m][0],Times_Memoires[m][1]);


				if(wc_ci_prefix(line,"DOWN") && (line[4]==' '||line[4]=='\t'))
					{
					down=(float)strtof(line+5,NULL);
					Times_Memoires[cue][3]=down;

					}
				if(wc_ci_prefix(line,"UP") && (line[2]==' '||line[2]=='\t'))
					{
					up=(float)strtof(line+3,NULL);
					Times_Memoires[cue][1]=up;

					}
				if(strncmp(line,"$$WAIT",6)==0)
					{
					autogotime=(float)strtof(line+7,NULL);
					if (autogotime>0) {Links_Memoires[cue]=1;}
				//	Times_Memoires[cue][0]=autogotime;
				//	Times_Memoires[cue][2]=autogotime;
					}

				// [ascii interop] niveaux d'intensite. Eos ecrit "$$ChanMove" (moves, pleine resolution
				// 16 bit) PUIS "Chan" (etat tracke, 8 bit). WhiteCat n'ecrivait que "CHAN 1/Hff" (8 bit).
				// On lit les deux : $$ChanMove -> 16 bit ecrit directement (source la plus precise) ;
				// Chan -> ne remplit QUE les circuits encore a 0 (ne clobbe pas le 16 bit du ChanMove).
				{
				bool _is_move = wc_ci_prefix(line,"$$CHANMOVE");
				bool _is_chan = _is_move ? false : wc_ci_prefix(line,"CHAN");
				if(_is_move || _is_chan)
					{
					char _lc[512]; strncpy(_lc,line,511); _lc[511]='\0';
					char* _t = strtok(_lc," \t\r\n"); // 1er token = mot-cle (Chan / $$ChanMove)
					_t = strtok(NULL," \t\r\n");
					while(_t!=NULL)
						{
						int _c=0,_l=0;
						if(wc_parse_chan_level(_t,&_c,&_l) && _c>0 && _c<513)
							{
							if(_is_move){ Memoires[cue][_c]=(unsigned short)_l; }
							else if(Memoires[cue][_c]==0){ Memoires[cue][_c]=(unsigned short)_l; }
							}
						_t = strtok(NULL," \t\r\n");
						}
					}
				}
                }//fin cues

                // [master import Congo] $MASTPAGEITEM <page> <item> ... : un submaster suivi de lignes
                // "CHAN <circuit>/H<niveau>". Mapping human-centric : item 1-40 -> fader 0-39,
                // page 1-6 -> dock 0-5 (une page de masters Congo = une banque de faders WCat).
                if (wc_ci_prefix(line,"$MASTPAGEITEM"))
                {
                    int _pg=0,_it=0;
                    sscanf(line,"%*s %d %d",&_pg,&_it);   // saute le mot-cle, lit page puis item
                    master_fader=_it-1; master_dock=_pg-1;
                    flagcue=-1; flagsub=-1; flagpatch=-1;
                    if(master_fader>=0 && master_fader<48 && master_dock>=0 && master_dock<6)
                    {
                        flagmaster=1;
                        for(int _cc=0;_cc<514;_cc++){ FaderDockContains[master_fader][master_dock][_cc]=0; }
                        DockTypeIs[master_fader][master_dock]=0; // dock de circuits (comme un sub)
                    }
                    else { flagmaster=-1; master_fader=-1; master_dock=-1; }
                }
                else if (wc_ci_prefix(line,"$MASTPAGE")) { flagmaster=-1; flagcue=-1; flagsub=-1; flagpatch=-1; }

                if (flagmaster==1 && wc_ci_prefix(line,"CHAN"))
                {
                    char _lc[512]; strncpy(_lc,line,511); _lc[511]='\0';
                    char* _t=strtok(_lc," \t\r\n"); _t=strtok(NULL," \t\r\n"); // saute "CHAN"
                    while(_t!=NULL)
                    {
                        int _c=0,_l=0;
                        if(wc_parse_chan_level(_t,&_c,&_l) && _c>0 && _c<514)
                        { FaderDockContains[master_fader][master_dock][_c]=(unsigned char)wc::lvl_to_dmx8((unsigned short)_l); }
                        _t=strtok(NULL," \t\r\n");
                    }
                }

                if (strncmp(line, "SET DEFAULT PATCH",17)==0 || strncmp(line, "CLEAR PATCH",11)==0  )
                {

                flagcue=-1; flagsub=-1; flagmaster=-1;
                flagpatch=1;
                if (strncmp(line, "SET DEFAULT PATCH",17)==0 )
                {
                patch_to_default_selected();
                }
                else if (strncmp(line, "CLEAR PATCH",11)==0 )
                {
                patch_clear_selected();
                }
                }
                if (flagpatch==1)//patch
                {
 				    if(strncmp(line,"PATCH 1",7)==0)
					{
					temp= strtok(line+7," ");
					while((temp!=NULL) && (strcmp(temp,"\n")!=0))
					    	{
							sscanf(temp,"%d<%d\n",&chan,&dimmer);
                            if(dimmer<513 && chan<513)
                            {
							Patch[dimmer]=chan;
                            }
							temp=strtok(NULL," ");

                            }
					}
                }//fin patch


                //subs

				if (strncmp(line, "SUB",3)==0)
					{
						sub= (int)strtof(line+4,NULL);
						flagsub=1;flagcue=-1;flagpatch=-1;
                    }

                if(flagsub==1)
                {
                int sub_f=0; int sub_d=0;

                if(sub<50 && sub!=0)
                {
                sub_f=sub-1; sub_d=0;
                }
                else {
                     sub_f=((sub-1)%48);
                     sub_d=((sub-1)/48);
                     }


                 if(strncmp(line,"TEXT",4)==0)
                 {
                 strncpy(DockName[sub_f][sub_d], line+5, 49);
                 DockName[sub_f][sub_d][49]='\0';
                 }

				if(strncmp(line,"DOWN",4)==0)
					{
					down=(float)strtof(line+5,NULL);
					//sab 02/03/2014 IMPACT time_per_dock[sub_f][sub_d][3]==down;
					time_per_dock[sub_f][sub_d][3]=down;

					}
				if(strncmp(line,"UP",2)==0)
					{
					up=(float)strtof(line+3,NULL);
					time_per_dock[sub_f][sub_d][1]=up;

					}
				if(strncmp(line,"$$WAIT",6)==0)
					{
					autogotime=(float)strtof(line+7,NULL);
					time_per_dock[sub_f][sub_d][0]=autogotime;
					time_per_dock[sub_f][sub_d][2]=autogotime;
					}

				 if(strncmp(line,"CHAN",4)==0)
					{
                    DockTypeIs[sub_f][sub_d]=0;
					temp= strtok(line+5," ");
					while((temp!=NULL) && (strcmp(temp,"\n")!=0))
					    	{
							sscanf(temp,"%d/H%x\n",&chan,&level);//debug 3/12/14 christoph
                            if(chan<513)
                            {
                            FaderDockContains[sub_f][sub_d][chan]=(unsigned char)level ;
							temp=strtok(NULL," ");
                            }
                            }

					}

                }//fin subs

			}
		else break;
if(strcmp(line,"ENDDATA")==0){ok=0;}
}
while (ok!=0);

fclose(f);
}
// [fix patch import / fixtures] L'import a ecrit le patch dans les tableaux legacy (Patch[]).
// Sans cet appel, le modele Fixture wc_patch (source de verite) ignore le patch importe, et le
// prochain rebuild_patch_from_fixtures() (chargement patch, refresh...) ecrase Patch[] au patch
// du modele (droit/defaut). On capture donc le patch importe DANS le modele -> il survit et se sauve.
synthesize_fixtures_from_legacy();
// [fix affichage patch] rafraichir le cache "premier gradateur par circuit" (chiffres rouges
// "show first dimmer") depuis Patch[] : sinon il reste sur l'ancien patch (droit) apres import,
// contradictoire avec la fenetre Patch. Reconstruit maintenant que Patch[] est correct.
generate_channel_preview_patch_list();
scan_for_free_dock();
////////////////////////////////////////////////////////////////////////////////
//detect sequenciel
position_onstage=first_cue_from_import;
refresh_mem_onstage(position_onstage);
detect_mem_before_one();
detect_mem_preset();
refresh_mem_onpreset(position_preset);
index_window_sequentiel=1;
////////////////////////////////////////////////////////////////////////////////
//REROLL
sprintf(rep,"%s",mondirectory);
chdir (rep);
index_is_saving=0;
return(0);
}


int do_ASCII_export()
{

index_is_saving=1;
rest(100);
sprintf(rep,"%s/import_export/ascii",mondirectory);

chdir(rep);


FILE *fp=NULL;


if((fp=fopen(importfile_name,"wt")))
{
    // [ascii interop] en-tete USITT standard lisible par Eos/Cobalt : "Ident 3:0" (deux-points !),
    // Manufacturer/Console, commentaires prefixes '!'. (Avant : "IDENT 3.0" + version nue = non standard.)
    fprintf(fp,"Ident 3:0\n");
    fprintf(fp,"Manufacturer ARPSCHUINO (White Cat)\n");
    fprintf(fp,"Console White Cat\n");
    fprintf(fp,"$$Software Version %s\n",versionis);

fprintf(fp,"Clear All\n\n");
int s=0;
int m=0;
int level_export=0;

 save_load_print_to_screen("memoires");

    for (m=0; m<10000; m++)
    {
        if (MemoiresExistantes[m]==1)
                {
                // [ascii interop] "Cue <num> <cuelist>" (Eos). memoire m -> cue (m/10).(m%10) list 1.
                fprintf(fp,"\nCue %d.%d 1\n", (m/10),(m%10));

                fprintf(fp,"Text %s\n", descriptif_memoires[m]);
                ////0=DIN  1=IN 2=DOUT 3=OUT
                fprintf(fp,"Down %.1f %.1f\n", Times_Memoires[m][3], Times_Memoires[m][2]);
                fprintf(fp,"Up %.1f %.1f\n", Times_Memoires[m][1], Times_Memoires[m][0]);
                if (Links_Memoires[m]==0) { fprintf(fp,"$$WAIT 0\n");}
                if (Links_Memoires[m]==1) { fprintf(fp,"$$WAIT 0.1\n");}

                // [ascii interop] deux lignes, comme Eos :
                //  $$ChanMove = pleine resolution 16 bit (4 chiffres hex) -> Eos + WhiteCat la relisent,
                //  Chan       = etat 8 bit standard USITT (2 chiffres hex) -> toute console la lit.
                fprintf(fp,"$$ChanMove ");
                for ( s=1;s<513;s++)
                                {
                                if (Memoires[m][s]>0)
                                                                {
                                                                fprintf(fp," %d@H%04x",s,(unsigned)Memoires[m][s]);
                                                                }
                                }
                 fprintf(fp,"\nChan ");
                for ( s=1;s<513;s++)
                                {
                                if (Memoires[m][s]>0)
                                                                {
                                                                level_export=(int) wc::lvl_to_dmx8(Memoires[m][s]);
                                                                fprintf(fp," %d@H%02x",s, level_export );
                                                                }
                                }

                 fprintf(fp,"\n");
                 }

     }
fprintf(fp,"\n");
fprintf(fp,"CLEAR PATCH\n");
fprintf(fp,"\nPATCH 1 ");

int retour_ligne=0;

for (int cpatch=1; cpatch<513; cpatch++)
{
for (int cpatch2=1;cpatch2<513;cpatch2++)
{
if (Patch[cpatch2]==cpatch)
{
fprintf (fp," %d<%d@100 ", cpatch, cpatch2);
retour_ligne++;
if (retour_ligne>5)
{
fprintf (fp,"\nPATCH 1 ");
retour_ligne=0;
}
}
}
}
/////////////////////FADERS/////////////////////////////////////////////////////

fprintf (fp,"\n");
fprintf (fp,"\nCLEAR SUBMASTERS");


for(int d=0;d<6;d++)
{
for(int f=0;f<48;f++)
{
if( DockTypeIs[f][d]==0)
{
fprintf (fp,"\n");
fprintf (fp,"SUB %d\n",(f+(48*d)+1));
fprintf(fp,"UP %.1f\n",time_per_dock[f][d][1]);
fprintf(fp,"DOWN %.1f\n",time_per_dock[f][d][3]);

fprintf(fp,"CHAN ");
for ( s=1;s<513;s++)
{
if (FaderDockContains[f][d][s]!=0 )
{
level_export=(int)(FaderDockContains[f][d][s]);
fprintf(fp,"%d/H%x ",s, level_export ); //essai level
}
}
fprintf(fp,"\n");
}
}
}

fprintf(fp,"\n\nENDDATA");
}
fclose(fp);

////////////////////////////////////////////////////////////////////////////////
//REROLL
sprintf(rep,"%s",mondirectory);
chdir (rep);
index_is_saving=0;

return(0);
}


/////////////////////////////////////////////////////////////////////////////////
///////////////////SCHWZ/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
int do_Schwartzpeter_import()
{
rest(10);
index_is_saving=1;
sprintf(rep,"%s/import_export/schwz/%s",mondirectory,importfile_name);
chdir(rep);

////////////////////////////////////////////////////////////////////////////////

FILE *fpscwz=NULL;

if ((fpscwz=fopen(f_cues, "rb"))==NULL)
{ sprintf(string_save_load_report[1],"Error opening file %s", f_cues); b_report_error[0]=1;}
else {
if (fread(SchwzMemoires, sizeof(int), SchwzMemoires_size, fpscwz) !=SchwzMemoires_size)
{ sprintf(string_save_load_report[1],"Error reading file %s", f_cues); b_report_error[0]=1;}
else {sprintf(string_save_load_report[1],"Schwartzpeter's Memories imported");}
}
///////////////////
if ((fpscwz=fopen(f_boolcues, "rb"))==NULL)
{ sprintf(string_save_load_report[2],"Error opening file %s", f_boolcues); b_report_error[2]=1;}
else
{
if (fread(SchwzMemoiresExistantes, sizeof(bool), SchwzMemoiresExistantes_size, fpscwz) !=SchwzMemoiresExistantes_size)
{ sprintf(string_save_load_report[2],"Error reading file %s", f_boolcues); b_report_error[2]=1;}
else {sprintf(string_save_load_report[2],"Schwartzpeter's Existing Memories imported");}
}
///////////////////
if ((fpscwz=fopen(f_autogo, "rb"))==NULL)
{ sprintf(string_save_load_report[3],"Error opening file %s", f_autogo); b_report_error[3]=1;}
else {
if (fread(Schwzautogo, sizeof(bool), Schwzautogo_size, fpscwz) !=Schwzautogo_size)
{ sprintf(string_save_load_report[3],"Error reading file %s", f_autogo); b_report_error[3]=1;}
else {sprintf(string_save_load_report[3],"Schwartzpeter's Autopo imported");}
}
///////////////////
if ((fpscwz=fopen(f_times, "rb"))==NULL)
{ sprintf(string_save_load_report[4],"Error opening file %s", f_times); b_report_error[4]=1;}
else
{
if (fread(SchwzMemTime, sizeof(float), SchwzMemTime_size, fpscwz) !=SchwzMemTime_size)
{ sprintf(string_save_load_report[4],"Error reading file %s", f_times); b_report_error[4]=1;}
else {sprintf(string_save_load_report[4],"Schwartzpeter's Times Memories imported");}
}
///////////////////
if ((fpscwz=fopen(f_patch, "rb"))==NULL)
{ sprintf(string_save_load_report[5],"Error opening file %s", f_patch); b_report_error[5]=1;}
else
{
if (fread(SchwzPatch, sizeof(int), SchwzPatch_size, fpscwz) !=SchwzPatch_size)
{ sprintf(string_save_load_report[5],"Error reading file %s", f_patch); b_report_error[5]=1;}
else {sprintf(string_save_load_report[5],"Schwartzpeter's Patch imported");}
}
///////////////////
if ((fpscwz=fopen(f_descriptif, "rb"))==NULL)//?
{ sprintf(string_save_load_report[6],"Error opening file %s", f_descriptif); b_report_error[6]=1;}
else
{
if (fread(Schwzdescriptif_mem, sizeof(char), Schwzdescriptif_size, fpscwz) !=Schwzdescriptif_size)
{ sprintf(string_save_load_report[6],"Error reading file %s", f_descriptif); b_report_error[6]=1;}
else {sprintf(string_save_load_report[6],"Schwartzpeter's Memories Description imported");}
}
///////////////////
if ( (fpscwz=fopen(f_shadow_sub, "rb") )==NULL)
{ sprintf(string_save_load_report[7],"Error opening file %s", f_shadow_sub); b_report_error[7]=1;}
else
{
if ( fread(Schwzshadow_sub, sizeof(int), Schwzsize_of_shadow_sub, fpscwz) != Schwzsize_of_shadow_sub)
{ sprintf(string_save_load_report[7],"Error reading file %s", f_shadow_sub); b_report_error[7]=1;}
else {sprintf(string_save_load_report[1],"Schwartzpeter ShadowSubs imported");}
}
fclose(fpscwz);
//////////////////////////REDISTRIBUTION////////////////////////////////////////
int schwzmem_before=0;
for (int m=1;m<5000;m++)
{
MemoiresExistantes[m]=SchwzMemoiresExistantes[m];
for (int c=0;c<121;c++)
{
Memoires[m][c]=wc::dmx8_to_lvl((unsigned char)(SchwzMemoires[c][m]*2.55));// [mem16 s1] import Schwz 0-100 -> 16 bit
if(SchwzMemoires[c][m]==100){Memoires[m][c]=wc::dmx8_to_lvl(255);}// [mem16 s1]
}
Links_Memoires[m]=Schwzautogo[m];
Times_Memoires[m][0]=SchwzMemTime[m][3];
Times_Memoires[m][1]=SchwzMemTime[m][1];
Times_Memoires[m][2]=SchwzMemTime[schwzmem_before][2];
Times_Memoires[m][3]=SchwzMemTime[schwzmem_before][0];

for(int t=0;t<11;t++)
{
if(Schwzdescriptif_mem[m][t]=='\0'){break;}
else {descriptif_memoires[m][t]=Schwzdescriptif_mem[m][t];}
}
descriptif_memoires[m][23]='\0';//christoph 16/06/14 debug instead of "\n"
if(SchwzMemoiresExistantes[m]==1) {schwzmem_before=m;}
}


for (int di=0;di<513;di++)
{
Patch[di]=SchwzPatch[di];
}
//reports shadows subs
for (int f=0;f<8;f++)
{
for(int f2=1;f2<6;f2++)
{
DockIsSelected[f*f2][0]=1;
DockTypeIs[f*f2][0]=0;
for (int co=0;co<121;co++)
{
 FaderDockContains[f*f2][0][co]=(unsigned char)Schwzshadow_sub[f][f2-1][co];
}
}
}
////////////////////////////////////////////////////////////////////////////////
//REROLL
sprintf(rep,"%s",mondirectory);
chdir (rep);

index_is_saving=0;
 return(0);
}


///////////////////////////////STRAND IMPORT////////////////////////////////////
int do_Alq_import()
{
index_is_saving=1;
sprintf(rep,"%s/import_export/ascii",mondirectory);
chdir(rep);
////////////////////////////////////////////////////////////////////////////////

	char line [256];
//sab 02/03/2014 unused var	int n=0;
	int cue=0;
	float down=0.0;
	float up=0.0;
	int chan=0;
	int level=0;//garder en int sinon la lecture ne se fait pas !
	float autogotime=0.0;
    int dimmer=0;
//sab 02/03/2014 unused var	bool alarm_cue=0; bool alarm_patch_chan=0; bool alarm_patch_dim=0;
	char *temp;
//sab 02/03/2014 unused var	char *string_text;
	int sub=0;
	int ok= -1;
	int flagcue= -1;
	int flagpatch= -1;
	int flagsub=-1;
//sab 02/03/2014 unused	int flaggroup=-1;
	int first_cue_from_import=0;//detect sequenciel



	FILE *f=NULL;


    if ((f=fopen(importfile_name, "rt"))== NULL)
    {
      sprintf(string_save_load_report[0],"Error opening %s",importfile_name); b_report_error[0]=1;

    }
    else
    {
     sprintf(string_save_load_report[0],"Opening %s",importfile_name);
	do {
		if (fgets(line,256,f)!=NULL)
		{
        //KEYWORD
                //CUE
				if (strncmp(line, "Cue",3)==0)
					{

						float tmpcueval= strtof(line+4,NULL);
						cue = (int)(tmpcueval*10);
						MemoiresExistantes[cue]=1;
	                    if(first_cue_from_import==0){first_cue_from_import=cue;}
						flagcue=1;flagsub=-1;flagpatch=-1;	//sab 02/03/2014 unused	flaggroup=-1;


                    }
                //Patch et action
 			    if(strncmp(line,"Patch 1",7)==0)
					{
                    sprintf(string_save_load_report[2],"Importing Patch");
    	            flagcue=-1;flagsub=-1;flagpatch=1;	//sab 02/03/2014 unused	flaggroup=-1;

                }//fin patch


                //subs
				if (strncmp(line, "Sub",3)==0)
					{	sprintf(string_save_load_report[3],"Importing Subs");
						sub= (int)strtof(line+4,NULL);
						flagsub=1;flagcue=-1;flagpatch=-1;	//sab 02/03/2014 unused	flaggroup=-1;
                    }

                 //group
				if (strncmp(line, "Group",4)==0)
				{
                  sprintf(string_save_load_report[4],"Importing Groups");
                  //sab 02/03/2014 unused	flaggroup=1;
                  flagcue=1;flagsub=-1;flagpatch=-1;	}



	    //DO CUES
        	    if(flagcue==1)//cues
			    {
                 sprintf(string_save_load_report[1],"Importing Cues");
                 if(strncmp(line,"Text",4)==0)
                 {
                 for (int p=0; p<24;p++)
                 {
                 descriptif_memoires[cue][p]=line[p+5];
                 }
                 descriptif_memoires[cue][24]='\0';
                 }
                 if(strncmp(line,"Up",2)==0)
					{
					up=(float)strtof(line+3,NULL);
					Times_Memoires[cue][3]=up;
	                Times_Memoires[cue][1]=up;
					}
				if(strncmp(line,"Down",4)==0)
					{
					down=(float)strtof(line+5,NULL);
					Times_Memoires[cue][1]=down;
					}

				if(strncmp(line,"$$WAIT",6)==0)
					{
					autogotime=(float)strtof(line+7,NULL);
					if (autogotime>0) {Links_Memoires[cue]=1;}
					Times_Memoires[cue][0]=autogotime;
					Times_Memoires[cue][2]=autogotime;
					}

				if(strncmp(line,"Chan",4)==0)
					{
					temp= strtok(line+5," ");
					while((temp!=NULL) && (strcmp(temp,"\n")!=0))
					    	{
							sscanf(temp,"%d=%d\n",&chan,&level);
							Memoires[cue][chan]=wc::dmx8_to_lvl((unsigned char)(level*2.55)) ;// [mem16 s1] import % -> 16 bit
							temp=strtok(NULL," ");
						    }
					}
                }//fin cues

               if (strncmp(line, "SET DEFAULT PATCH",17)==0 || strncmp(line, "CLEAR PATCH",11)==0  )
                {
                flagcue=-1; flagsub=-1;
                flagpatch=1;
                if (strncmp(line, "SET DEFAULT PATCH",17)==0 )
                {
                patch_to_default_selected();
                }
                else if (strncmp(line, "CLEAR PATCH",11)==0 )
                {
                patch_clear_selected();
                }
                flagpatch=0;
                }


                //DO JOB PATCH
                if(flagpatch==1)
                {

    	        if(strncmp(line,"Patch 1",7)==0)
				{
               //sab 02/03/2014 unused var char temp_ch[24],temp_dim[24];
					temp= strtok(line+7," ");
					while((temp!=NULL) && (strcmp(temp,"\n")!=0))
					    	{
                            char temp2[24];
                            sscanf(temp,"  %s",temp2);
							sscanf(temp2,"%d<<%d=100",&chan,&dimmer);
						    if(chan<513 && dimmer <513)
						    {
							Patch[dimmer]=chan;
                            }
                            temp=strtok(NULL,"  ");
                            }
                            }
              }
              //DO JOB SUBS
                if(flagsub==1)
                {
                int sub_f=0; int sub_d=0;

                if(sub<50 && sub!=0)
                {
                sub_f=sub-1; sub_d=0;
                }
                else {
                     sub_f=((sub-1)%48);
                     sub_d=((sub-1)/48);
                     }


                 if(strncmp(line,"Text",4)==0)
                 {
                 strncpy(DockName[sub_f][sub_d], line+5, 49);
                 DockName[sub_f][sub_d][49]='\0';
                 }


				if(strncmp(line,"Up",2)==0)
					{
					up=(float)strtof(line+3,NULL);
					time_per_dock[sub_f][sub_d][1]=up;
	                time_per_dock[sub_f][sub_d][3]=up;
					}
                if(strncmp(line,"Down",4)==0)
					{
					down=(float)strtof(line+5,NULL);
					//sab 02/03/2014 IMPACT time_per_dock[sub_f][sub_d][3]==down;
					time_per_dock[sub_f][sub_d][3]=down;

					}
				if(strncmp(line,"$$WAIT",6)==0)
					{
					autogotime=(float)strtof(line+7,NULL);
					time_per_dock[sub_f][sub_d][0]=autogotime;
					time_per_dock[sub_f][sub_d][2]=autogotime;
					}

				 if(strncmp(line,"Chan",4)==0)
					{
                    DockTypeIs[sub_f][sub_d]=0;
					temp= strtok(line+5," ");
					while((temp!=NULL) && (strcmp(temp,"\n")!=0))
					    	{
							sscanf(temp,"%d/%d\n",&chan,&level);
                            FaderDockContains[sub_f][sub_d][chan]=(unsigned char)(level*2.55) ;
							temp=strtok(NULL," ");
						    }
					}

                }//fin subs

			}
		else break;
if(strcmp(line,"ENDDATA")==0){ok=0;}
}
while (ok!=0);

fclose(f);
}
scan_for_free_dock();
///remise en ordre du sequenciel///////////////////////////////////////////////
position_onstage=first_cue_from_import;
refresh_mem_onstage(position_onstage);
detect_mem_before_one();
detect_mem_preset();
refresh_mem_onpreset(position_preset);
////////////////////////////////////////////////////////////////////////////////
//REROLL
sprintf(rep,"%s",mondirectory);
chdir (rep);
index_is_saving=0;
return(0);
}
