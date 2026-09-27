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

* \file patch_splines.cpp
* \brief {Curves calcul for each canal in the patch}
* \author Christoph Guillermet
* \version {0.8.6.3}
* \date {12/02/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Fonction de calcul des courbes dans le patch dmx
*
*   Calcul fonction of the curves in the dmx patch
*
**/

#include "wc_tus.h"

/* calculates the distance between two curve_nodes */
fixed curve_node_dist(curve_node n1, curve_node n2)
{
   #define SCALE  64

   fixed dx = itofix(n1.x - n2.x) / SCALE;
   fixed dy = itofix(n1.y - n2.y) / SCALE;

   return fixsqrt(fixmul(dx, dx) + fixmul(dy, dy)) * SCALE;
}


/* constructs curve_nodes to go at the ends of the list, for tangent calculations */
curve_node dummy_curve_node(curve_node curve_Node, curve_node prev)
{
   curve_node n;

   n.x = curve_Node.x - (prev.x - curve_Node.x) / 8;
   n.y = curve_Node.y - (prev.y - curve_Node.y) / 8;
   n.tangent = itofix(0);

   return n;
}



/* calculates a set of curve_node tangents */
void curve_calc_tangents(void)
{

     //pas touche prepa tangentes OK pour sens en avant
   int i;

   curve_nodes[0] = dummy_curve_node(curve_nodes[1], curve_nodes[2]);
   curve_nodes[curve_node_count] = dummy_curve_node(curve_nodes[curve_node_count-1], curve_nodes[curve_node_count-2]);

   curve_node_count++;

  for (i=1; i<curve_node_count-1; i++)
      curve_nodes[i].tangent = fixatan2(itofix(curve_nodes[i+1].y - curve_nodes[i-1].y),
				  itofix(curve_nodes[i+1].x - curve_nodes[i-1].x));



}




/* calculates the control points for a spline segment */
void curve_get_control_points(curve_node n1, curve_node n2, int points[8])
{
   fixed dist = fixmul(curve_node_dist(n1, n2), curve_curviness);

   points[0] = n1.x;
   points[1] = n1.y;

   points[2] = n1.x + fixtoi(fixmul(fixcos(n1.tangent), dist));
   points[3] = n1.y + fixtoi(fixmul(fixsin(n1.tangent), dist));

   points[4] = n2.x - fixtoi(fixmul(fixcos(n2.tangent), dist));
   points[5] = n2.y - fixtoi(fixmul(fixsin(n2.tangent), dist));

   points[6] = n2.x;
   points[7] = n2.y;
}



int write_curve()
{
index_writing_curve=1;
 int points[MAX_curve_nodeS ];// etait [8] le 24 aout


 // [fix courbe FF] curve_calc_tangents() incremente curve_node_count a CHAQUE appel : appele ici
 // pendant un glisser, il faisait calculer des segments parasites au-dela du point 5 (noeuds
 // fantomes, voire perimes / hors tableau), clampes sur l'index 255 -> la valeur de FF dependait
 // de ces restes (ex. circuits eteints a FF). On fige le compteur (5 points reels + fantomes),
 // on ne calcule que les 4 vrais segments, et on le restaure ensuite.
 int save_node_count = curve_node_count;
 curve_node_count = 6;
 curve_curviness = ftofix(curve_spline_level);
 curve_calc_tangents();// noeuds fantomes 0 et 6, curve_node_count -> 7

 for(int nio=1;nio<5;nio++)
 {
 curve_get_control_points(curve_nodes[nio],curve_nodes[nio+1],points);

 int resolu= (curve_nodes[nio+1].x) - (curve_nodes[nio].x);
 if(resolu<1){continue;}// points confondus / croises : pas de segment (evitait un tableau de taille <= 0)
 if(resolu>300){resolu=300;}
 int temp_curve_x[301];
 int temp_curve_y[301];
 calc_spline(points,resolu,  temp_curve_x, temp_curve_y);

 int index_sp=0;

 for(int cuv=0;cuv<resolu;cuv++)
 {
 index_sp=(curve_nodes[nio].x+cuv)-(xpatch_window+30+455);

 if(index_sp>255){index_sp=255;}
 if(index_sp<0){index_sp=0;}

 curve_report[curve_selected][index_sp]=temp_curve_y[cuv]-(ypatch_window+50);

 if(curve_report[curve_selected][index_sp]<0){curve_report[curve_selected][index_sp]=0;}
 if(curve_report[curve_selected][index_sp]>255){curve_report[curve_selected][index_sp]=255;}

 }
 }
 // [fix courbe FF] du point 5 jusqu'a 255 inclus : valeur du point 5 (jamais ecrite avant ->
 // FF gardait un reste d'un etat anterieur de la courbe).
 {
 int last_y=curve_ctrl_pt[curve_selected][5][1];
 if(last_y<0){last_y=0;}
 if(last_y>255){last_y=255;}
 int from_x=curve_ctrl_pt[curve_selected][5][0];
 if(from_x<0){from_x=0;}
 if(from_x>255){from_x=255;}
 for(int ix=from_x;ix<=255;ix++){curve_report[curve_selected][ix]=last_y;}
 }
 curve_node_count = save_node_count;
index_writing_curve=0;
 return(0);
}





int draw_curve_node(int n)
{
   if(n<6){
   Circle(curve_nodes[n].x, curve_nodes[n].y, 6).DrawOutline(CouleurBlind);
   petitchiffre.Print(ol::ToString(n),curve_nodes[n].x-7, curve_nodes[n].y-7);
   }


   // [fix courbe drag] capture : le point saisi suit la souris tant que le bouton est enfonce,
   // meme si un mouvement rapide la fait sortir de la petite zone de survol (le point "decrochait").
   static int s_curve_drag_node = 0; // 0 = aucun point saisi
   if(!(mouse_b&1)){ s_curve_drag_node = 0; }

   // [fix courbe drag] survol = le point le plus proche de la souris parmi ceux dont la colonne
   // (+/- diam/2) est sous la souris : le plus proche en hauteur, puis en largeur. Avant, il fallait
   // aussi etre a diam/2 au-dela des voisins -> deux points colles sur la meme verticale devenaient
   // impossibles a saisir.
   int curve_hover_n = 0;
   if( window_focus_id==W_PATCH
   && mouse_x>=xpatch_window+30+455-(diam_curve_node/2) && mouse_x<=xpatch_window+30+455+255+(diam_curve_node/2)
   && mouse_y>=ypatch_window+50 && mouse_y<=ypatch_window+255+50 )
   {
     int best_dy=100000, best_dx=100000;
     for(int k=1;k<=5;k++)
     {
       int dx=mouse_x-curve_nodes[k].x; if(dx<0){dx=-dx;}
       if(dx>(diam_curve_node/2)){continue;}
       int dy=mouse_y-curve_nodes[k].y; if(dy<0){dy=-dy;}
       if(dy<best_dy || (dy==best_dy && dx<best_dx)){best_dy=dy; best_dx=dx; curve_hover_n=k;}
     }
   }
   bool curve_node_hover = (curve_hover_n==n);

   if(curve_node_hover && (mouse_b&1) && index_enable_curve_editing==1 && s_curve_drag_node==0 && n>=1 && n<=5)
   { s_curve_drag_node = n; }

   bool curve_node_dragged = (s_curve_drag_node==n && (mouse_b&1) && index_enable_curve_editing==1 && window_focus_id==W_PATCH);

   if( curve_node_hover || curve_node_dragged )
   {
     Line( Vec2D( curve_nodes[n].x, ypatch_window+50 ), Vec2D(curve_nodes[n].x,ypatch_window+255+50)).Draw(Rgba::YELLOW);
     Circle(curve_nodes[n].x, curve_nodes[n].y, 6).Draw(CouleurBlind);

    if( curve_node_dragged )
    {
    // position locale bornee : y dans le cadre, x entre les voisins (pas de croisement de points)
    int loc_y=mouse_y-(ypatch_window+50);
    if(loc_y<0){loc_y=0;}
    if(loc_y>255){loc_y=255;}
    if(n>1 && n<5)
    {
    int loc_x=mouse_x-(xpatch_window+30+455);
    int min_x=curve_ctrl_pt[curve_selected][n-1][0]+1;
    int max_x=curve_ctrl_pt[curve_selected][n+1][0]-1;
    if(loc_x<min_x){loc_x=min_x;}
    if(loc_x>max_x){loc_x=max_x;}
    curve_ctrl_pt[curve_selected][n][0]=loc_x;
    curve_ctrl_pt[curve_selected][n][1]=loc_y;
    }
    if(n==1)
    {
    curve_ctrl_pt[curve_selected][n][0]=0;
    curve_ctrl_pt[curve_selected][n][1]=loc_y;
    }
    if(n==5)
    {
    curve_ctrl_pt[curve_selected][n][0]=255;
    curve_ctrl_pt[curve_selected][n][1]=loc_y;
    }

    write_curve();//ecriture des niveaux
    }

   }

   if (dmx_view==1)//255
   {
   //affichage OUT
   petitpetitchiffrerouge.Print(ol::ToString(255-curve_ctrl_pt[curve_selected][n][1]),xpatch_window+5+455,ypatch_window+50+curve_ctrl_pt[curve_selected][n][1]);
   //affichage IN
   petitpetitchiffre.Print(ol::ToString(curve_ctrl_pt[curve_selected][n][0]),xpatch_window+455+20+curve_ctrl_pt[curve_selected][n][0],ypatch_window+50+270);
   }
   else   if (dmx_view==0)//%
   {
   //affichage OUT
   petitpetitchiffrerouge.Print(ol::ToString((int)((255-curve_ctrl_pt[curve_selected][n][1])/2.55)),xpatch_window+5+455,ypatch_window+50+curve_ctrl_pt[curve_selected][n][1]);
   //affichage IN
   petitpetitchiffre.Print(ol::ToString((int)((curve_ctrl_pt[curve_selected][n][0])/2.55)),xpatch_window+455+20+curve_ctrl_pt[curve_selected][n][0],ypatch_window+50+270);
   }


 return(0);
}



/* draws the spline paths */
int curve_draw_splines()
{
    int io;
     // [fix courbe FF] le noeud fantome 0 est deja pose par curve_calc_tangents() ; l ancienne ligne
     // lisait curve_nodes[curve_node_count+1] = case 8, hors du tableau (MAX_curve_nodeS = 8).
   for (io=1; io<curve_node_count-1; io++)
   {
      curve_nodes[io].x=(curve_ctrl_pt[curve_selected][io][0]+xpatch_window+30+455);
      curve_nodes[io].y=(curve_ctrl_pt[curve_selected][io][1]+ypatch_window+50);
      draw_curve_node(io);
   }

return(0);
}


int view_curve_after_draw()//verif du report de ma courbe
{
for (int d=0; d<=255;d++)// [fix courbe FF] inclut 255 (FF) dans l apercu
{
 Point(xpatch_window+30+455+d,ypatch_window+50+curve_report[curve_selected][d] ).Draw(Rgba::GREEN);
}
   return(0);
}


int SplineCurve()
{

curve_node_count=6;
curve_curviness = ftofix(curve_spline_level);
curve_calc_tangents();
//allegro_gl_set_allegro_mode(); //melange gl et allegro screen
curve_draw_splines();//l attribution des pas est fait dans drawsplines
//allegro_gl_unset_allegro_mode(); //melange gl et allegro screen
view_curve_after_draw();
return(0);
}


int build_square_curve(int curve)
{
 //points
 curve_ctrl_pt[curve][1][0]=0; curve_ctrl_pt[curve][1][1]=255;//point 1 en 0 0

  curve_ctrl_pt[curve][2][0]=27; curve_ctrl_pt[curve][2][1]=255-89;//point 1 en 0 0
  curve_ctrl_pt[curve][3][0]=55; curve_ctrl_pt[curve][3][1]=255-153;//point 1 en 0 0
  curve_ctrl_pt[curve][4][0]=118; curve_ctrl_pt[curve][4][1]=255-213;//point 1 en 0 0

 curve_ctrl_pt[curve][5][0]=255; curve_ctrl_pt[curve][5][1]=0;//point 5 en 255 255

 the_curve_spline_level[curve]=176;
 index_curve_spline_level=176;
 curve_spline_level=(((float)index_curve_spline_level)/127)-1;
 curve_node_count=6;
 curve_curviness = ftofix(curve_spline_level);
 curve_calc_tangents();
 curve_draw_splines();
//write_curve(); //fait planter si debordement de memoire
 view_curve_after_draw();
 return(0);
}


int build_fluo_curve(int curve)
{
 //points
 curve_ctrl_pt[curve][1][0]=0; curve_ctrl_pt[curve][1][1]=255;//point 1 en 0 0

  curve_ctrl_pt[curve][2][0]=12; curve_ctrl_pt[curve][2][1]=255-45;//point 1 en 0 0
  curve_ctrl_pt[curve][3][0]=45; curve_ctrl_pt[curve][3][1]=255-103;//point 1 en 0 0
  curve_ctrl_pt[curve][4][0]=157; curve_ctrl_pt[curve][4][1]=255-178;//point 1 en 0 0

 curve_ctrl_pt[curve][5][0]=255; curve_ctrl_pt[curve][5][1]=0;//point 5 en 255 255

 the_curve_spline_level[curve]=176;
 index_curve_spline_level=176;
 curve_spline_level=(((float)index_curve_spline_level)/127)-1;
 curve_node_count=6;
 curve_curviness = ftofix(curve_spline_level);
 curve_calc_tangents();
 curve_draw_splines();
//write_curve(); //fait planter si debordement de memoire
 view_curve_after_draw();
 return(0);
}

int build_preheat_curve(int curve)
{
 //points
 curve_ctrl_pt[curve][1][0]=0; curve_ctrl_pt[curve][1][1]=255-28;//point 1 en 0 0

  curve_ctrl_pt[curve][2][0]=40; curve_ctrl_pt[curve][2][1]=255-86;//point 1 en 0 0
  curve_ctrl_pt[curve][3][0]=102; curve_ctrl_pt[curve][3][1]=255-163;//point 1 en 0 0
  curve_ctrl_pt[curve][4][0]=187; curve_ctrl_pt[curve][4][1]=255-235;//point 1 en 0 0

 curve_ctrl_pt[curve][5][0]=255; curve_ctrl_pt[curve][5][1]=0;//point 5 en 255 255

 the_curve_spline_level[curve]=176;
 index_curve_spline_level=178;
 curve_spline_level=(((float)index_curve_spline_level)/127)-1;
 curve_node_count=6;
 curve_curviness = ftofix(curve_spline_level);
 curve_calc_tangents();
 curve_draw_splines();
//write_curve(); //fait planter si debordement de memoire
 view_curve_after_draw();
 return(0);
}


int build_inverse_curve(int curve)
{
 //points
 for (int pt=1;pt<MAX_curve_nodeS-1;pt++)
 {
  curve_ctrl_pt[curve][pt][0]=(int)((((float)255)/6) *pt);
  curve_ctrl_pt[curve][pt][1]=(int)((((float)255)/6) *pt);
 }
 curve_ctrl_pt[curve][1][0]=0; curve_ctrl_pt[curve][1][1]=0;//point 1 en 0 0
 curve_ctrl_pt[curve][5][0]=255; curve_ctrl_pt[curve][5][1]=255;//point 5 en 255 255
 the_curve_spline_level[curve]=168;
 index_curve_spline_level=168;
 curve_spline_level=(((float)index_curve_spline_level)/127)-1;
 curve_node_count=6;
curve_curviness = ftofix(curve_spline_level);
curve_calc_tangents();
curve_draw_splines();
//write_curve(); //fait planter si debordement de memoire
view_curve_after_draw();
 return(0);
}


// ============================================================
// Initialisation des courbes par defaut (correction de fond)
// ------------------------------------------------------------
// compute_curve_report_local : calcule curve_report[c] a partir de
// curve_ctrl_pt[c] + the_curve_spline_level[c], en coordonnees locales (0..255),
// SANS dependance a la fenetre patch ni au rendu (contrairement a write_curve()).
// Reproduit le meme calcul : tangentes -> points de Bezier -> echantillonnage spline.
// Les offsets xpatch_window/ypatch_window de write_curve() s'annulent : on travaille
// directement en 0..255. Validee : ecart < 2 vs la courbe square editee a la main.
static void compute_curve_report_local(int c)
{
    // sauvegarde de l'etat global manipule
    int        save_sel = curve_selected;
    int        save_nc  = curve_node_count;
    float      save_sl  = curve_spline_level;
    fixed      save_cv  = curve_curviness;
    curve_node save_nodes[MAX_curve_nodeS];
    for (int i = 0; i < MAX_curve_nodeS; i++) save_nodes[i] = curve_nodes[i];

    // noeuds en coordonnees locales (1..5)
    for (int i = 1; i <= 5; i++) {
        curve_nodes[i].x = curve_ctrl_pt[c][i][0];
        curve_nodes[i].y = curve_ctrl_pt[c][i][1];
        curve_nodes[i].tangent = 0;
    }
    curve_node_count   = 6;
    curve_spline_level = ((float)the_curve_spline_level[c] / 127.0f) - 1.0f;
    curve_curviness    = ftofix(curve_spline_level);
    curve_calc_tangents();   // noeuds fantomes + tangentes + curve_node_count++

    for (int i = 0; i < 256; i++) curve_report[c][i] = 0;

    int points[8];
    for (int nio = 1; nio < (curve_node_count - 1); nio++) {
        curve_get_control_points(curve_nodes[nio], curve_nodes[nio + 1], points);
        int resolu = curve_nodes[nio + 1].x - curve_nodes[nio].x;
        if (resolu < 1)   resolu = 1;
        if (resolu > 300) resolu = 300;
        int tx[301], ty[301];
        calc_spline(points, resolu, tx, ty);
        for (int cuv = 0; cuv < resolu; cuv++) {
            int idx = curve_nodes[nio].x + cuv;   // coords locales 0..255
            if (idx < 0)   idx = 0;
            if (idx > 255) idx = 255;
            int val = ty[cuv];
            if (val < 0)   val = 0;
            if (val > 255) val = 255;
            curve_report[c][idx] = val;
        }
    }
    // [fix courbe FF] du point 5 jusqu a 255 inclus : valeur du point 5 (comme write_curve)
    {
        int last_y = curve_ctrl_pt[c][5][1];
        if (last_y < 0)   last_y = 0;
        if (last_y > 255) last_y = 255;
        int from_x = curve_ctrl_pt[c][5][0];
        if (from_x < 0)   from_x = 0;
        if (from_x > 255) from_x = 255;
        for (int ix = from_x; ix <= 255; ix++) curve_report[c][ix] = last_y;
    }

    // restauration de l'etat global
    curve_selected     = save_sel;
    curve_node_count   = save_nc;
    curve_spline_level = save_sl;
    curve_curviness    = save_cv;
    for (int i = 0; i < MAX_curve_nodeS; i++) curve_nodes[i] = save_nodes[i];
}

// init_default_curves : pose les courbes par defaut (poignees + report coherents)
// 0 et 4..15 = lineaire, 1 = square, 2 = preheat, 3 = fluo.
// Ne (re)genere QU'UNE courbe dont curve_report est entierement nul (invalide) :
// une courbe valide chargee d'un show ou personnalisee est respectee.
// A appeler juste apres Load_Show() : repare aussi les last_save corrompus.
int init_default_curves()
{
    for (int c = 0; c < 16; c++) {
        bool nulle = true;
        for (int i = 0; i < 256; i++) { if (curve_report[c][i] != 0) { nulle = false; break; } }
        if (!nulle) continue;   // courbe valide -> on n'y touche pas

        if (c == 1) {           // square
            curve_ctrl_pt[c][1][0]=0;   curve_ctrl_pt[c][1][1]=255;
            curve_ctrl_pt[c][2][0]=27;  curve_ctrl_pt[c][2][1]=255-89;
            curve_ctrl_pt[c][3][0]=55;  curve_ctrl_pt[c][3][1]=255-153;
            curve_ctrl_pt[c][4][0]=118; curve_ctrl_pt[c][4][1]=255-213;
            curve_ctrl_pt[c][5][0]=255; curve_ctrl_pt[c][5][1]=0;
            the_curve_spline_level[c]=176;
            compute_curve_report_local(c);
        } else if (c == 2) {    // preheat
            curve_ctrl_pt[c][1][0]=0;   curve_ctrl_pt[c][1][1]=255-28;
            curve_ctrl_pt[c][2][0]=40;  curve_ctrl_pt[c][2][1]=255-86;
            curve_ctrl_pt[c][3][0]=102; curve_ctrl_pt[c][3][1]=255-163;
            curve_ctrl_pt[c][4][0]=187; curve_ctrl_pt[c][4][1]=255-235;
            curve_ctrl_pt[c][5][0]=255; curve_ctrl_pt[c][5][1]=0;
            the_curve_spline_level[c]=178;
            compute_curve_report_local(c);
        } else if (c == 3) {    // fluo
            curve_ctrl_pt[c][1][0]=0;   curve_ctrl_pt[c][1][1]=255;
            curve_ctrl_pt[c][2][0]=12;  curve_ctrl_pt[c][2][1]=255-45;
            curve_ctrl_pt[c][3][0]=45;  curve_ctrl_pt[c][3][1]=255-103;
            curve_ctrl_pt[c][4][0]=157; curve_ctrl_pt[c][4][1]=255-178;
            curve_ctrl_pt[c][5][0]=255; curve_ctrl_pt[c][5][1]=0;
            the_curve_spline_level[c]=176;
            compute_curve_report_local(c);
        } else {                // lineaire (0, 4..15)
            curve_ctrl_pt[c][1][0]=0;   curve_ctrl_pt[c][1][1]=255;
            curve_ctrl_pt[c][2][0]=64;  curve_ctrl_pt[c][2][1]=255-64;
            curve_ctrl_pt[c][3][0]=128; curve_ctrl_pt[c][3][1]=255-128;
            curve_ctrl_pt[c][4][0]=192; curve_ctrl_pt[c][4][1]=255-192;
            curve_ctrl_pt[c][5][0]=255; curve_ctrl_pt[c][5][1]=0;
            the_curve_spline_level[c]=168;
            for (int i = 0; i < 256; i++) curve_report[c][i] = 255 - i;
            curve_report[c][255] = 0;
        }
    }
    return 0;
}
