/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
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

* \file wizard_operations.cpp
* \brief {GUI fonctions for wizard }
* \author Christoph Guillermet
* \version {0.8.6.3}
* \date {12/02/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Core fonctions pour la fenêtre wizard, il s'agit d'un outil permettant de faire des changement, copier coler, changement d'etat etc.. dans toutes les mémoires de la conduite ou partielement
*
*   Core fonctions for the wizard window, a tool to change parameter easly in all memories or group of memories
*
**/

#include "wc_tus.h"
#include <zlib.h>   // [undo] copie compressee des memoires
#include "fixturectl_visu.h"   // [devices] attribut -> output (Wizard CH)

int wizard_detect_unused_channels(int wizard_from_mem, int wizard_to_mem)
{
bool used_channel[514];
for(int look=wizard_from_mem;look<=wizard_to_mem;look++)
{
if(MemoiresExistantes[look]==1)
{
for (int l=1;l<514;l++)
{
if(Memoires[look][l]>0)
{
used_channel[l]=1;
}
}
}
}
//report sur selection
for(int l=1;l<514;l++)
{
Selected_Channel[l]=1-used_channel[l];
}

return(0);
}


int wizard_calcul_nbre_de_mem()
{
   nbre_de_mems_manipulees=0;
   for(int memo=wizard_from_mem;memo<=wizard_to_mem;memo++)
   {
   if(MemoiresExistantes[memo]==1){nbre_de_mems_manipulees++;}
   }
 return(0);
}

int wizard_calcul_nbre_de_mem_deleted()
{
   nbre_de_mems_manipulees=0;
   for(int memo=wizard_from_mem;memo<=wizard_to_mem;memo++)
   {
   if(MemoiresDetruites[memo]==1){nbre_de_mems_manipulees++;}
   }

return(0);
}



int wizard_rebuild(int facteur_memoires)
{
int mem_p=wizard_from_mem;

for(int look=wizard_from_mem;look<=wizard_to_mem;look++)
{
if(Wiz_MemoiresExistantes[look]==1)
{
MemoiresExistantes[mem_p]=Wiz_MemoiresExistantes[look];
for(int cho=0;cho<513;cho++)
{Memoires[mem_p][cho]=Wiz_Memoires[look][cho];}
for(int o=0;o<514;o++){Memoires_devval[mem_p][o]=Wiz_Memoires_devval[look][o];} // [devices] attributs des fixtures suivent la memoire
for(int t=0;t<4;t++)
{Times_Memoires[mem_p][t]=Wiz_Times_Memoires[look][t];}
Links_Memoires[mem_p]=Wiz_Links_Memoires[look];
Banger_Memoire[mem_p]=Wiz_Banger_Memoire[look];
sprintf(descriptif_memoires[mem_p],Wiz_descriptif_memoires[look]);
sprintf(annotation_memoires[mem_p],Wiz_annotation_memoires[look]);

Wiz_MemoiresExistantes[look]=0;
mem_p+=facteur_memoires;
}

}
position_onstage=wizard_from_mem;
refresh_mem_onstage(position_onstage);
detect_mem_before_one();
detect_mem_preset();
refresh_mem_onpreset(position_preset);

 return(0);
}


int wizard_clear_transfert_array()
{
for(int pm=wizard_from_mem;pm<=wizard_to_mem;pm++)
{
Wiz_MemoiresExistantes[pm]=0;
for(int cho=0;cho<513;cho++)
{
Wiz_Memoires[pm][cho]=0;
}
for(int o=0;o<514;o++){Wiz_Memoires_devval[pm][o]=0;} // [devices] attributs des fixtures suivent la memoire
Wiz_Times_Memoires[pm][0]=0.0;
Wiz_Times_Memoires[pm][1]=0.0;
Wiz_Times_Memoires[pm][2]=0.0;
Wiz_Times_Memoires[pm][3]=0.0;
Wiz_Links_Memoires[pm]=0;
Wiz_Banger_Memoire[pm]=0;
strcpy(Wiz_descriptif_memoires[pm],"");
strcpy(Wiz_annotation_memoires[pm],"");
Wiz_descriptif_memoires[pm][49]='\0';
Wiz_annotation_memoires[pm][49]='\0';
}
 return(0);
}

int wizard_clear_mem_original()
{
for(int pm=wizard_from_mem;pm<=wizard_to_mem;pm++)
{
MemoiresExistantes[pm]=0;
for(int cho=0;cho<513;cho++)
{
Memoires[pm][cho]=0;
}
for(int o=0;o<514;o++){Memoires_devval[pm][o]=0;} // [devices] attributs des fixtures suivent la memoire
Times_Memoires[pm][0]=0.0;
Times_Memoires[pm][1]=0.0;
Times_Memoires[pm][2]=0.0;
Times_Memoires[pm][3]=0.0;
Links_Memoires[pm]=0;
Banger_Memoire[pm]=0;
strcpy(descriptif_memoires[pm],"");
descriptif_memoires[pm][24]='\0';
strcpy(annotation_memoires[pm],"");
annotation_memoires[pm][24]='\0';
}
 return(0);
}

int wizard_delete_mem()
{
for(int pm=wizard_from_mem;pm<=wizard_to_mem;pm++)
{
MemoiresExistantes[pm]=0;
MemoiresDetruites[pm]=1;
}
 return(0);
}


int wizard_restore_mem()
{
for(int pm=wizard_from_mem;pm<=wizard_to_mem;pm++)
{
if(MemoiresDetruites[pm]==1){MemoiresExistantes[pm]=1;MemoiresDetruites[pm]=0;}
}
 return(0);
}

int wizard_copy_mem()
{
wizard_clear_transfert_array();
for(int pm=wizard_from_mem;pm<=wizard_to_mem;pm++)
{
Wiz_MemoiresExistantes[pm]=MemoiresExistantes[pm];
for(int cho=0;cho<513;cho++)
{
Wiz_Memoires[pm][cho]=Memoires[pm][cho];
}
for(int o=0;o<514;o++){Wiz_Memoires_devval[pm][o]=Memoires_devval[pm][o];} // [devices] attributs des fixtures suivent la memoire
for(int t=0;t<4;t++)
{
Wiz_Times_Memoires[pm][t]=Times_Memoires[pm][t];
}
Wiz_Links_Memoires[pm]=Links_Memoires[pm];
Wiz_Banger_Memoire[pm]=Banger_Memoire[pm];
sprintf(Wiz_descriptif_memoires[pm],descriptif_memoires[pm]);
sprintf(Wiz_annotation_memoires[pm],annotation_memoires[pm]);
}
 return(0);
}

int wizard_inspekt_mem()
{
strcpy(tmp_wizbuffer,"");
char tmpw[8];

for (int w=1;w<513;w++)
{
if(Memoires[variable_wizard_mem][w]>0)
{
switch(dmx_view)
{
case 0:
sprintf(tmpw,"%d=%d ",w,wc::lvl_to_pct(Memoires[variable_wizard_mem][w]));// [mem16] % depuis 16 bit
break;
case 1:
sprintf(tmpw,"%d=%d ",w,wc::lvl_to_dmx8(Memoires[variable_wizard_mem][w]));// [mem16] DMX 8 bit depuis 16 bit
break;
}
strcat(tmp_wizbuffer, tmpw);
}
}

//splittage chaine caracters
for (int ly=0;ly<36;ly++)
{
strcpy(show_buff_wizOUT[ly],"");
for (int by=0;by<71;by++)
{
show_buff_wizOUT[ly][by]=tmp_wizbuffer[(ly*71)+by];
}
show_buff_wizOUT[ly][72]='\0';
}


return(0);
}

int wizard_insert_mems()
{
//nettoyage dans conduite des memoires en position d insertion
for(int pm=wizard_from_mem;pm<=wizard_from_mem+variable_wizard_mem;pm++)
{
MemoiresExistantes[pm]=0;
for(int cho=0;cho<513;cho++)
{
Memoires[pm][cho]=0;
}
for(int o=0;o<514;o++){Memoires_devval[pm][o]=0;} // [devices] attributs des fixtures suivent la memoire
Times_Memoires[pm][0]=0.0;
Times_Memoires[pm][1]=0.0;
Times_Memoires[pm][2]=0.0;
Times_Memoires[pm][3]=0.0;
Links_Memoires[pm]=0;
Banger_Memoire[pm]=0;
strcpy(descriptif_memoires[pm],"");
descriptif_memoires[pm][24]='\0';
strcpy(annotation_memoires[pm],"");
annotation_memoires[pm][24]='\0';
}
//recopie

for(int look=wizard_from_mem;look<=wizard_to_mem;look++)
{
if(Wiz_MemoiresExistantes[look]==1 && look+variable_wizard_mem<10000)
{
MemoiresExistantes[look+variable_wizard_mem]=Wiz_MemoiresExistantes[look];
for(int cho=0;cho<513;cho++)
{Memoires[look+variable_wizard_mem][cho]=Wiz_Memoires[look][cho];}
for(int o=0;o<514;o++){Memoires_devval[look+variable_wizard_mem][o]=Wiz_Memoires_devval[look][o];} // [devices] attributs des fixtures suivent la memoire
for(int t=0;t<4;t++)
{Times_Memoires[look+variable_wizard_mem][t]=Wiz_Times_Memoires[look][t];}
Links_Memoires[look+variable_wizard_mem]=Wiz_Links_Memoires[look];
Banger_Memoire[look+variable_wizard_mem]=Wiz_Banger_Memoire[look];
sprintf(descriptif_memoires[look+variable_wizard_mem],Wiz_descriptif_memoires[look]);
sprintf(annotation_memoires[look+variable_wizard_mem],Wiz_annotation_memoires[look]);
Wiz_MemoiresExistantes[look]=0;
}
}
position_onstage=wizard_from_mem;
refresh_mem_onstage(position_onstage);
detect_mem_before_one();
detect_mem_preset();
refresh_mem_onpreset(position_preset);


return(0);
}


int wizard_move_mems()
{
//backup de la conduite en entier
wizard_clear_transfert_array();

//sab 02/03/2014 set but not used bool Wiz_backup_exist_second[10000];
for(int pm=0;pm<10000;pm++)
{
Wiz_MemoiresExistantes[pm]=MemoiresExistantes[pm];
//sab 02/03/2014 set but not used Wiz_backup_exist_second[pm]=MemoiresExistantes[pm];
for(int cho=0;cho<513;cho++)
{
Wiz_Memoires[pm][cho]=Memoires[pm][cho];
}
for(int o=0;o<514;o++){Wiz_Memoires_devval[pm][o]=Memoires_devval[pm][o];} // [devices] attributs des fixtures suivent la memoire
for(int t=0;t<4;t++)
{
Wiz_Times_Memoires[pm][t]=Times_Memoires[pm][t];
}
Wiz_Links_Memoires[pm]=Links_Memoires[pm];
Wiz_Banger_Memoire[pm]=Banger_Memoire[pm];
sprintf(Wiz_descriptif_memoires[pm],descriptif_memoires[pm]);
sprintf(Wiz_annotation_memoires[pm],annotation_memoires[pm]);
}

//effacement de l existence de ces memoires
for(int pm=wizard_from_mem;pm<=wizard_to_mem;pm++)
{
MemoiresExistantes[pm]=0;
for(int cho=0;cho<513;cho++)
{
Memoires[pm][cho]=0;
}
for(int o=0;o<514;o++){Memoires_devval[pm][o]=0;} // [devices] attributs des fixtures suivent la memoire
Times_Memoires[pm][0]=0.0;
Times_Memoires[pm][1]=0.0;
Times_Memoires[pm][2]=0.0;
Times_Memoires[pm][3]=0.0;
Links_Memoires[pm]=0;
Banger_Memoire[pm]=0;
strcpy(descriptif_memoires[pm],"");
descriptif_memoires[pm][24]='\0';
strcpy(annotation_memoires[pm],"");
annotation_memoires[pm][24]='\0';
}

//recopie des memoires à insérer
int index_mem=0;
int fin_des_memoires=0;
for(int look=wizard_from_mem;look<=wizard_to_mem;look++)
{
if((variable_wizard_mem+index_mem)<10000)
{
MemoiresExistantes[variable_wizard_mem+index_mem]=Wiz_MemoiresExistantes[look];
for(int cho=0;cho<513;cho++)
{Memoires[variable_wizard_mem+index_mem][cho]=Wiz_Memoires[look][cho];}
for(int o=0;o<514;o++){Memoires_devval[variable_wizard_mem+index_mem][o]=Wiz_Memoires_devval[look][o];} // [devices] attributs des fixtures suivent la memoire
for(int t=0;t<4;t++)
{Times_Memoires[variable_wizard_mem+index_mem][t]=Wiz_Times_Memoires[look][t];}
Links_Memoires[variable_wizard_mem+index_mem]=Wiz_Links_Memoires[look];
Banger_Memoire[variable_wizard_mem+index_mem]=Wiz_Banger_Memoire[look];
sprintf(descriptif_memoires[variable_wizard_mem+index_mem],Wiz_descriptif_memoires[look]);
sprintf(annotation_memoires[variable_wizard_mem+index_mem],Wiz_annotation_memoires[look]);
index_mem++;
fin_des_memoires=variable_wizard_mem+index_mem;
}
}
//le recopiage et decalage du reste
//arrondissage index_mem pour garder la logique du numérotage des .
index_mem/=10;
index_mem+=1;
index_mem*=10;
////////////////////////////////////////////
for(int look=fin_des_memoires;look<10000;look++)
{
if((look-index_mem)>0)
{
MemoiresExistantes[look]=Wiz_MemoiresExistantes[look-index_mem];
for(int cho=0;cho<513;cho++)
{Memoires[look][cho]=Wiz_Memoires[look-index_mem][cho];}
for(int o=0;o<514;o++){Memoires_devval[look][o]=Wiz_Memoires_devval[look-index_mem][o];} // [devices] attributs des fixtures suivent la memoire
for(int t=0;t<4;t++)
{Times_Memoires[look][t]=Wiz_Times_Memoires[look-index_mem][t];}
Links_Memoires[look]=Wiz_Links_Memoires[look-index_mem];
Banger_Memoire[look]=Wiz_Banger_Memoire[look-index_mem];
sprintf(descriptif_memoires[look],Wiz_descriptif_memoires[look-index_mem]);
sprintf(annotation_memoires[look],Wiz_annotation_memoires[look-index_mem]);
}
}

position_onstage=variable_wizard_mem;
refresh_mem_onstage(position_onstage);
detect_mem_before_one();
detect_mem_preset();
refresh_mem_onpreset(position_preset);

return(0);
}

int wizard_reset_to_default_time()
{
for(int pm=wizard_from_mem;pm<=wizard_to_mem;pm++)
{
if(MemoiresExistantes[pm]==1)
{
Times_Memoires[pm][0]=0.0;
Times_Memoires[pm][2]=0.0;
Times_Memoires[pm][1]=default_time;//0=DIN 1=IN 2=DOUT 3=OUT
Times_Memoires[pm][3]=default_time;
}
}

return(0);
}



///////////////////////////////////////////////////////////////////////////////
int do_wizard_mem()
{
if(wizard_amplitude_is_global==1)
{
bool firstmem=0;
for(int m=0;m<10000;m++)
{
if(MemoiresExistantes[m]==1 && firstmem==0)
{
firstmem=1;
wizard_from_mem=m;
}
if(MemoiresExistantes[m]==1 && firstmem!=0)
{
wizard_to_mem=m;
}
}
}

switch(wizard_action_mem) // 0 rebuild numerical order / 1 insert me / 2 del mem / 3 move mem /
//4 inspekt delmem / 5 restore del mem / 6 set times / 7 reset times, links, bangers /
{
   case 0:// 0 rebuild numerical order
   wizard_copy_mem();
   wizard_calcul_nbre_de_mem();
   wizard_clear_mem_original();
   switch(mode_rebuild)
   {
   case 0:
   wizard_rebuild(1);
   break;
   case 1:
   wizard_rebuild(10);
   break;
   case 2:
   wizard_rebuild(100);
   break;
   }
   break;
   case 1:// 1 insert mem
   wizard_to_mem=9999;
   wizard_copy_mem();
   wizard_calcul_nbre_de_mem();
   wizard_clear_mem_original();
   wizard_insert_mems();
   break;
   case 2:// 2 del mem
   wizard_calcul_nbre_de_mem();
   wizard_delete_mem();
   break;
   case 3:// 3 move mem
   wizard_calcul_nbre_de_mem();
   wizard_move_mems();
   break;
   case 4://4 inspekt delmem
   wizard_inspekt_mem();
   break;
   case 5:// 5 restore del mem
   wizard_calcul_nbre_de_mem_deleted();
   wizard_restore_mem();
   break;
   case 6:// 6 reset times
   wizard_reset_to_default_time();
   break;
}

return(0);
}



// [devices] Wizard CH sur un attribut de fixture (wizard_attr_key) : Set/Add/Reduce de la valeur
// enregistree (Memoires_devval, 16 bit par output coarse) dans les memoires de la plage.
// Les circuits sans cet attribut sont ignores. Valeur saisie en % (ou 0-255 en vue DMX).
static void do_wizard_ch_attribute()
{
int outs[514]; int nouts=0;
for(int co=1;co<513;co++)
{
 if(wizard_buffer_in[co]==1)
 {
  int o=fxc_attr_output_for_circuit(co,wizard_attr_key);
  if(o>0 && o<514){ outs[nouts++]=o; }
 }
}
int v;
if(dmx_view==0){ int p=wizard_level_is; if(p>100){p=100;} if(p<0){p=0;} v=wc::pct_to_lvl(p); }
else { int d=wizard_level_is; if(d>255){d=255;} if(d<0){d=0;} v=wc::dmx8_to_lvl((unsigned char)d); }
for(int m=wizard_from_mem;m<=wizard_to_mem;m++)
{
 if(m<0 || m>9999 || MemoiresExistantes[m]!=1) continue;
 for(int i=0;i<nouts;i++)
 {
  int cur=Memoires_devval[m][outs[i]];
  switch(wizard_action)
  {
  case 0: cur=v; break;                 // Set at
  case 1: cur+=v; if(cur>65535){cur=65535;} break;   // Add (meme a 0 : 0 est une vraie valeur d'attribut)
  case 2: cur-=v; if(cur<0){cur=0;} break;           // Reduce
  }
  Memoires_devval[m][outs[i]]=(unsigned short)cur;
 }
}
}

int do_wizard_ch()
{
if(wizard_amplitude_is_global==1)
{
bool firstmem=0;
for(int m=0;m<10000;m++)
{
if(MemoiresExistantes[m]==1 && firstmem==0)
{
firstmem=1;
wizard_from_mem=m;
}
if(MemoiresExistantes[m]==1 && firstmem!=0)
{
wizard_to_mem=m;
}
}
}


if(wizard_attr_key[0]!=0 && wizard_action<=2)
{
do_wizard_ch_attribute();
refresh_stage();
if(index_go==0 && index_go_back==0 && index_pause==0){ refresh_mem_onpreset(position_preset); }
return(0);
}

switch(wizard_action) // 0 Set at level / 1 Add / 2 Reduce / 3 replace
{
case 0://Set at level
for(int m=wizard_from_mem;m<=wizard_to_mem;m++)
{
if(MemoiresExistantes[m]==1)
{
for(int co=1;co<513;co++)
{
if(wizard_buffer_in[co]==1)
{
switch(dmx_view)
{
case 0: // [mem16 stage C] Set en % -> 16 bit natif (pct_to_lvl, plus de quantification 8 bit)
if(wizard_level_is>0)
{
int _p=wizard_level_is; if(_p>100){_p=100;}
Memoires[m][co]=wc::pct_to_lvl(_p);
}
else if(wizard_level_is==0)
{
Memoires[m][co]=0;
}
break;
case 1: // [mem16 stage C] Set en DMX 8 bit (0-255) -> 16 bit (x257)
Memoires[m][co]=wc::dmx8_to_lvl((unsigned char)wizard_level_is);
break;
}
}
}
}
}
break;
case 1://Add
for(int m=wizard_from_mem;m<=wizard_to_mem;m++)
{
if(MemoiresExistantes[m]==1)
{
for(int co=1;co<513;co++)
{
if(wizard_buffer_in[co]==1)
{
if(Memoires[m][co]>0)//on ne rajoute pas sur un circuit à 0%
{
switch(dmx_view)
{
case 0:
{ // [mem16 stage C] Add en % : math 16 bit sans round-trip 8 bit (garde la finesse du niveau courant)
int _cur=Memoires[m][co]; int _p=wizard_level_is; if(_p>100){_p=100;} int _a=wc::pct_to_lvl(_p);
if(_cur+_a<=65535){Memoires[m][co]=(unsigned short)(_cur+_a);}
else {Memoires[m][co]=wc::LVL_MAX;}
}
break;
case 1:
{ // [mem16 stage C] Add en DMX : delta = wizard_level_is x257, math 16 bit
int _cur=Memoires[m][co]; int _a=wc::dmx8_to_lvl((unsigned char)wizard_level_is);
if(_cur+_a<=65535){Memoires[m][co]=(unsigned short)(_cur+_a);}
else  {Memoires[m][co]=wc::LVL_MAX;}
}
break;
}
}
}
}
}
}
break;
case 2://Reduce
for(int m=wizard_from_mem;m<=wizard_to_mem;m++)
{
if(MemoiresExistantes[m]==1)
{
for(int co=1;co<513;co++)
{
if(wizard_buffer_in[co]==1)
{
if(Memoires[m][co]>0)//on ne retire pas sur un circuit à 0%
{
switch(dmx_view)
{
case 0:
{ // [mem16 stage C] Reduce en % : math 16 bit sans round-trip 8 bit
int _cur=Memoires[m][co]; int _p=wizard_level_is; if(_p>100){_p=100;} int _s=wc::pct_to_lvl(_p);
if(_cur-_s>=0){Memoires[m][co]=(unsigned short)(_cur-_s);}
else {Memoires[m][co]=0;}
}
break;
case 1:
{ // [mem16 stage C] Reduce en DMX : delta = wizard_level_is x257, math 16 bit
int _cur=Memoires[m][co]; int _s=wc::dmx8_to_lvl((unsigned char)wizard_level_is);
if(_cur-_s>=0){Memoires[m][co]=(unsigned short)(_cur-_s);}
else {Memoires[m][co]=0;}
}
break;
}
}
}
}
}
}
break;
case 3://Exchange Replace
unsigned short tmp_buff_wiz[514];// [mem16] temp 16 bit (shift wizard sans perte)
bool please_replace[514];

for(int m=wizard_from_mem;m<=wizard_to_mem;m++)
{
if(MemoiresExistantes[m]==1)
{
int index_replace=0;
//salve stocke
for(int co=1;co<513;co++)
{
//init
please_replace[co]=0;
tmp_buff_wiz[co]=0;

if(wizard_buffer_in[co]==1)
{
tmp_buff_wiz[co]=Memoires[m][co];
please_replace[co]=1;
Memoires[m][co]=0;
strcpy(descriptif_projecteurs[co],"");
}
}
//salve redistribue
int index_nbre_circuit=0;

for(int co=1;co<513;co++)
{
if(wizard_buffer_out[co]==1)
{
for(int p=index_replace;p<513;p++)
{
        if(number_inchannels==number_outchannels)
        {
                         if(please_replace[p]==1)
                         {
                         Memoires[m][co]=tmp_buff_wiz[p];
                         please_replace[p]=0;
                         break;
                         }
        }

        else if(number_inchannels<number_outchannels )
        {

                        if(please_replace[p]==1)
                        {
                        Memoires[m][co]=tmp_buff_wiz[p];
                        index_nbre_circuit++;
                        if(index_nbre_circuit<number_inchannels)
                        {
                        please_replace[p]=0;
                        break;
                        }
                        }
        }
}
}
}
}
}

//channel macros



break;
//SWAP
case 4:
//swap
bool swapIN[514];
bool swapOUT[514];
unsigned short tmp_buff_mem[514];// [mem16] temp 16 bit
for(int m=wizard_from_mem;m<=wizard_to_mem;m++)
{
if(MemoiresExistantes[m]==1)
{
//salve recopie d exchange vers in
for(int co=1;co<513;co++)
{
tmp_buff_mem[co]=Memoires[m][co];
swapIN[co]=wizard_buffer_in[co];
swapOUT[co]=wizard_buffer_out[co];
}
for(int co=1;co<513;co++)
{
if(swapIN[co]==1)
{
for(int t=1;t<513;t++)
{
if(swapOUT[t]==1)
{
Memoires[m][co]=tmp_buff_mem[t];
swapOUT[t]=0;break;
}
}
}
}
//salve recopie de in vers out
for(int co=1;co<513;co++)
{
swapOUT[co]=wizard_buffer_out[co];
}
for(int co=1;co<513;co++)
{
if(swapOUT[co]==1)
{
for(int t=1;t<513;t++)
{
if(swapIN[t]==1)
{
Memoires[m][co]=tmp_buff_mem[t];
swapIN[t]=0;break;
}
}
}
}

}
}



break;
}

refresh_stage();


 return(0);
}

// ============================================================================
// [undo] Annulation de la derniere action du Wizard -- VRAI undo (seulement ce que le Wizard a change).
// 1. wizard_undo_snapshot() AVANT DO IT : toutes les memoires, enregistrement par enregistrement,
//    dans un fichier gzip temporaire (pas de RAM en plus).
// 2. wizard_undo_commit() APRES DO IT : relit cette copie et ne garde (fichier diff) que les memoires
//    modifiees par le Wizard : numero + etat avant + etat apres.
// 3. wizard_undo_restore() : pour chaque memoire du diff, si elle est encore dans l'etat "apres"
//    (pas retouchee depuis), elle revient a l'etat "avant" ; sinon elle est gardee (conflit compte).
// Un seul niveau. Invalide au chargement d'un show (Load_Show).
// ============================================================================
struct WizMemRec {
    unsigned short lev[514];
    unsigned short dev[514];
    float times[4];
    int banger;
    int ratio;
    char desc[50];
    char annot[50];
    unsigned char exists, links, destroyed, pad;
};
static void wiz_rec_get(int m, WizMemRec& r)
{
    memset(&r, 0, sizeof(r));
    memcpy(r.lev, Memoires[m], sizeof(r.lev));
    memcpy(r.dev, Memoires_devval[m], sizeof(r.dev));
    memcpy(r.times, Times_Memoires[m], sizeof(r.times));
    r.banger=Banger_Memoire[m]; r.ratio=ratio_cross_manuel[m];
    memcpy(r.desc, descriptif_memoires[m], sizeof(r.desc));
    memcpy(r.annot, annotation_memoires[m], sizeof(r.annot));
    r.exists=MemoiresExistantes[m]; r.links=Links_Memoires[m]; r.destroyed=MemoiresDetruites[m];
}
static void wiz_rec_set(int m, const WizMemRec& r)
{
    memcpy(Memoires[m], r.lev, sizeof(r.lev));
    memcpy(Memoires_devval[m], r.dev, sizeof(r.dev));
    memcpy(Times_Memoires[m], r.times, sizeof(r.times));
    Banger_Memoire[m]=r.banger; ratio_cross_manuel[m]=r.ratio;
    memcpy(descriptif_memoires[m], r.desc, sizeof(r.desc));
    memcpy(annotation_memoires[m], r.annot, sizeof(r.annot));
    MemoiresExistantes[m]=r.exists; Links_Memoires[m]=r.links; MemoiresDetruites[m]=r.destroyed;
}
static void wiz_undo_path(char* out, int n, const char* name)
{
    char t[400]; wc_get_temp_dir(t, sizeof(t));
    snprintf(out, n, "%s%s", t, name);
}
static int wiz_undo_pos_before[2] = {0,0};

int wizard_undo_snapshot()
{
    char path[512]; wiz_undo_path(path, sizeof(path), "whitecat_wizard_before.gz");
    gzFile gz=gzopen(path, "wb1");   // niveau 1 : rapide (memoires surtout a zero)
    if(gz==NULL){ sprintf(string_Last_Order,">> Wizard: undo copy failed (%s)", path); return(1); }
    wiz_undo_pos_before[0]=position_onstage; wiz_undo_pos_before[1]=position_preset;
    static WizMemRec r;
    bool ok=true;
    for(int m=0;m<10000 && ok;m++){ wiz_rec_get(m,r); if(gzwrite(gz,&r,sizeof(r))!=(int)sizeof(r)) ok=false; }
    gzclose(gz);
    if(!ok){ remove(path); }
    return(ok ? 0 : 1);
}

int wizard_undo_commit()
{
    char pb[512]; wiz_undo_path(pb, sizeof(pb), "whitecat_wizard_before.gz");
    char pd[512]; wiz_undo_path(pd, sizeof(pd), "whitecat_wizard_undo.gz");
    gzFile in=gzopen(pb, "rb");
    if(in==NULL){ return(1); }   // pas de copie avant : l'undo precedent reste valable
    gzFile out=gzopen(pd, "wb1");
    if(out==NULL){ gzclose(in); remove(pb); wizard_undo_available=0; return(1); }
    int pos[4]={wiz_undo_pos_before[0],wiz_undo_pos_before[1],position_onstage,position_preset};
    gzwrite(out,pos,sizeof(pos));
    static WizMemRec rb, ra;
    int changed=0; bool ok=true;
    for(int m=0;m<10000;m++)
    {
        if(gzread(in,&rb,sizeof(rb))!=(int)sizeof(rb)){ ok=false; break; }
        wiz_rec_get(m,ra);
        if(memcmp(&rb,&ra,sizeof(rb))!=0)
        {
            if(gzwrite(out,&m,sizeof(m))!=(int)sizeof(m) || gzwrite(out,&rb,sizeof(rb))!=(int)sizeof(rb) || gzwrite(out,&ra,sizeof(ra))!=(int)sizeof(ra)){ ok=false; break; }
            changed++;
        }
    }
    gzclose(in); gzclose(out); remove(pb);
    wizard_undo_available = (ok && changed>0) ? 1 : 0;
    return(ok ? 0 : 1);
}

int wizard_undo_restore()
{
    if(!wizard_undo_available) return(1);
    char pd[512]; wiz_undo_path(pd, sizeof(pd), "whitecat_wizard_undo.gz");
    gzFile gz=gzopen(pd, "rb");
    if(gz==NULL){ wizard_undo_available=0; sprintf(string_Last_Order,">> Wizard: undo copy not found"); return(1); }
    index_is_saving=1;            // fige les timers de fond pendant la reecriture des memoires
    wc_wait_background_idle();
    int pos[4]={0,0,0,0};
    bool ok = gzread(gz,pos,sizeof(pos))==(int)sizeof(pos);
    static WizMemRec rb, ra, rc;
    int restored=0, kept=0, m=0;
    while(ok && gzread(gz,&m,sizeof(m))==(int)sizeof(m))
    {
        if(gzread(gz,&rb,sizeof(rb))!=(int)sizeof(rb) || gzread(gz,&ra,sizeof(ra))!=(int)sizeof(ra) || m<0 || m>9999){ ok=false; break; }
        wiz_rec_get(m,rc);
        if(memcmp(&rc,&ra,sizeof(rc))==0){ wiz_rec_set(m,rb); restored++; }
        else { kept++; }          // retouchee depuis le Wizard : on garde la version actuelle
    }
    gzclose(gz);
    if(position_onstage==pos[2]){ position_onstage=pos[0]; }
    refresh_mem_onstage(position_onstage);
    detect_mem_before_one();
    if(position_preset==pos[3]){ position_preset=pos[1]; }
    refresh_mem_onpreset(position_preset);
    index_is_saving=0;
    wizard_undo_available=0;      // un seul niveau d'annulation
    if(!ok){ sprintf(string_Last_Order,">> Wizard: undo copy damaged !"); }
    else if(kept>0){ sprintf(string_Last_Order,">> Wizard undone: %d mems restored, %d edited since were kept",restored,kept); }
    else { sprintf(string_Last_Order,">> Wizard undone: %d mems restored",restored); }
    return(ok ? 0 : 1);
}
