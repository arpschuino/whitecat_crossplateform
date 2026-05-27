/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
       WWWWWWWWWWWWWWW           |               2026       Jacques Bouault - arpschuino.fr
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

* \file sequentiel_7_visu.cpp
* \brief {GUI fonctions for the cuelist}
* \author Christoph Guillermet
* \modified Jacques Bouault - arpschuino.fr - 2026
* \version {0.9.0}
* \date {2026}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   GUI fonctions pour la fenêtre des memoires
*
*   GUI fonctions for the cuelist
*
**/

#include "wc_tus.h"
#include "gui_boutons_rebuild1.h"
#include "grider_visu.h"

static void seq_draw_editbox(int x, int y) {
    const int inner_w = 184; // text area width

    // Measure cursor pixel position in petitchiffre
    char buf_cur[50];
    int cur_b = (seq_edit_cursor < 50) ? seq_edit_cursor : 49;
    memcpy(buf_cur, confirm_name_buf, cur_b);
    buf_cur[cur_b] = '\0';
    int cursor_px = (int)petitchiffre.TextWidth(buf_cur);

    // Scroll so cursor stays visible
    int scroll_x = 0;
    if (cursor_px > inner_w - 4) scroll_x = cursor_px - (inner_w - 4);

    // Box outline (slightly wider than inner area)
    Rect box(Vec2D(x - 4, y - 14), Vec2D(inner_w + 4, 20));
    box.SetLineWidth(1);
    box.DrawOutline(CouleurLigne);

    // Text clipped to inner area
    SDL_Rect clip = {x, y - 12, inner_w, 16};
    SDL_RenderSetClipRect(wc_sdl_renderer, &clip);
    petitchiffre.Print(confirm_name_buf, x + 2 - scroll_x, y);
    SDL_RenderSetClipRect(wc_sdl_renderer, NULL);

    // Blinking cursor
    if (alpha_blinker > 0.5f) {
        int cx = x + 2 + cursor_px - scroll_x;
        if (cx >= x - 2 && cx <= x + inner_w + 2)
            Line(Vec2D(cx, y - 11), Vec2D(cx, y + 1)).Draw(CouleurLigne);
    }
}

static void seq_print_clipped(const char *text, int x, int y) {
    char buf[50];
    strncpy(buf, text, 49);
    buf[49] = '\0';
    while (buf[0] && petitpetitchiffre.TextWidth(buf) > 184) {
        int len = (int)strlen(buf);
        int i = len - 1;
        while (i > 0 && ((unsigned char)buf[i] & 0xC0) == 0x80) i--;
        buf[i] = '\0';
    }
    petitpetitchiffre.Print(buf, x, y);
}

int refresh_vision_memories(int x_seq, int y_seq) {
    sprintf(string_mem_onstage, "%d.%d", position_onstage / 10, position_onstage % 10);
    sprintf(string_mem_preset,  "%d.%d", position_preset  / 10, position_preset  % 10);
    if (MemoiresExistantes[mem_before_one] == 1)
        sprintf(string_mem_before_one, "%d.%d", mem_before_one / 10, mem_before_one % 10);
    else
        sprintf(string_mem_before_one, "-");

    Rect ExclueMem(Vec2D(x_seq, y_seq), Vec2D(10, 30));

    if (window_focus_id == W_SEQUENCIEL && mouse_x > x_seq && mouse_x < x_seq + 450 && mouse_y > y_seq and
        mouse_y < y_seq + hauteur_globale_sequenciel) // affichage survol des cases souris
    {
        Rect DelayOver(Vec2D(x_seq, y_seq), Vec2D(40, 20));
        DelayOver.SetRoundness(7.5);
        Rect InOutOver(Vec2D(x_seq, y_seq), Vec2D(60, 20));
        InOutOver.SetRoundness(7.5);
        Rect MemOver(Vec2D(x_seq, y_seq), Vec2D(60, 20));
        MemOver.SetRoundness(7.5);
        Rect LinkOver(Vec2D(x_seq, y_seq), Vec2D(30, 20));
        LinkOver.SetRoundness(7.5);
        Rect bangOver(Vec2D(x_seq, y_seq), Vec2D(30, 20));
        bangOver.SetRoundness(7.5);
        Rect Gpl1Over(Vec2D(x_seq, y_seq), Vec2D(30, 20));
        Gpl1Over.SetRoundness(7.5);
        Rect TextOver(Vec2D(x_seq, y_seq), Vec2D(184, 15));
        TextOver.SetRoundness(5);

        // MANIPULATIONS MEM BEFORE ONE
        if (mouse_y > y_seq + 80 && mouse_y < y_seq + 100) {
            if (mouse_x > x_seq + 180 && mouse_x < x_seq + 213) // LINKS
            {
                LinkOver.MoveTo(Vec2D(x_seq + 180, y_seq + 80));
                LinkOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 213 && mouse_x < x_seq + 246) // BANGER
            {
                bangOver.MoveTo(Vec2D(x_seq + 215,y_seq + 80));
                bangOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 246 && mouse_x < x_seq + 280) // GRID PLAYER
            {
                Gpl1Over.MoveTo(Vec2D(x_seq + 248,y_seq + 80));
                Gpl1Over.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 286 && mouse_x < x_seq + 470) // TEXT DECRIPTION
            {
                if (mouse_y > y_seq + 80 && mouse_y < y_seq + 95) {
                    TextOver.MoveTo(Vec2D(x_seq + 286, y_seq + 80));
                } else {
                    TextOver.MoveTo(Vec2D(x_seq + 286, y_seq + 95));
                }
                TextOver.DrawOutline(CouleurLigne);
            }
        }

        // Manipulation en stage
        else if (mouse_y > y_seq + 110 && mouse_y < y_seq + 140) {
            if (mouse_x > x_seq + 10 && mouse_x < x_seq + 50) // DELAYS
            {
                DelayOver.MoveTo(Vec2D(x_seq + 10, y_seq + 115));
                DelayOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 50 && mouse_x < x_seq + 110) // IN OUT
            {
                InOutOver.MoveTo(Vec2D(x_seq + 50, y_seq + 115));
                InOutOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 110 && mouse_x < x_seq + 170) // MEMS
            {
                MemOver.MoveTo(Vec2D(x_seq + 110, y_seq + 115));
                MemOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 180 && mouse_x < x_seq + 213) // LINKS
            {
                LinkOver.MoveTo(Vec2D(x_seq + 180, y_seq + 115));
                LinkOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 213 && mouse_x < x_seq + 246) // BANGER
            {
                bangOver.MoveTo(Vec2D(x_seq + 215,y_seq + 115));
                bangOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 246 && mouse_x < x_seq + 280) // GRID PLAYER
            {
                Gpl1Over.MoveTo(Vec2D(x_seq + 248,y_seq + 115));
                Gpl1Over.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 286 && mouse_x < x_seq + 470) // TEXT DECRIPTION
            {
                if (mouse_y > y_seq + 110 && mouse_y < y_seq + 125) {
                    TextOver.MoveTo(Vec2D(x_seq + 286, y_seq + 110));
                } else {
                    TextOver.MoveTo(Vec2D(x_seq + 286, y_seq + 125));
                }
                TextOver.DrawOutline(CouleurLigne);
            }
        }

        // Manipulation en preset
        else if (mouse_y > y_seq + 140 && mouse_y < y_seq + 170) {
            if (mouse_x > x_seq + 10 && mouse_x < x_seq + 50) // DELAYS
            {
                DelayOver.MoveTo(Vec2D(x_seq + 10, y_seq + 145));
                DelayOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 50 && mouse_x < x_seq + 110) // IN OUT
            {
                InOutOver.MoveTo(Vec2D(x_seq + 50, y_seq + 145));
                InOutOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 110 && mouse_x < x_seq + 170) // MEMS
            {
                MemOver.MoveTo(Vec2D(x_seq + 110, y_seq + 145));
                MemOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 180 && mouse_x < x_seq + 213) // LINKS
            {
                LinkOver.MoveTo(Vec2D(x_seq + 180, y_seq + 145));
                LinkOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 213 && mouse_x < x_seq + 246) // BANGER
            {
                bangOver.MoveTo(Vec2D(x_seq + 215,y_seq + 145));
                bangOver.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 246 && mouse_x < x_seq + 280) // GRID PLAYER
            {
                Gpl1Over.MoveTo(Vec2D(x_seq + 248,y_seq + 145));
                Gpl1Over.DrawOutline(CouleurLigne);
            } else if (mouse_x > x_seq + 286 && mouse_x < x_seq + 470) // TEXT DECRIPTION
            {
                if (mouse_y > y_seq + 140 && mouse_y < y_seq + 155) {
                    TextOver.MoveTo(Vec2D(x_seq + 286, y_seq + 140));
                } else {
                    TextOver.MoveTo(Vec2D(x_seq + 286, y_seq + 155));
                }
                TextOver.DrawOutline(CouleurLigne);
            }
        }

        // Next Mems
        for (int i = 0; i < nbre_memoires_visualisables_en_preset; i++) {
            /// ACTIONS
            if (mouse_y > y_seq + 145 + (35 * i) && mouse_y < y_seq + 175 + (35 * i)) {

                if (mouse_x > x_seq + 110 && mouse_x < x_seq + 170) // MEMS
                {
                    MemOver.MoveTo(Vec2D(x_seq + 110, y_seq + 145 + (35 * i)));
                    MemOver.DrawOutline(CouleurLigne);
                } else if (mouse_x > x_seq + 180 && mouse_x < x_seq + 213) // LINKS
                {
                    LinkOver.MoveTo(Vec2D(x_seq + 180, y_seq + 145 + (35 * i)));
                    LinkOver.DrawOutline(CouleurLigne);
                } else if (mouse_x > x_seq + 213 && mouse_x < x_seq + 246) // BANGER
                {
                    bangOver.MoveTo(Vec2D(x_seq + 215,y_seq + 145 + (35 * i)));
                    bangOver.DrawOutline(CouleurLigne);
                } else if (mouse_x > x_seq + 246 && mouse_x < x_seq + 280) // GRID PLAYER
                {
                    Gpl1Over.MoveTo(Vec2D(x_seq + 248,y_seq + 145 + (35 * i)));
                    Gpl1Over.DrawOutline(CouleurLigne);
                } else if (mouse_x > x_seq + 286 && mouse_x < x_seq + 470) // TEXT DECRIPTION
                {
                    if (mouse_y > y_seq + 140 + (35 * i) && mouse_y < y_seq + 155 + (35 * i)) {
                        TextOver.MoveTo(Vec2D(x_seq + 286, y_seq + 140 + (35 * i)));
                    } else {
                        TextOver.MoveTo(Vec2D(x_seq + 286, y_seq + 155 + (35 * i)));
                    }
                    TextOver.DrawOutline(CouleurLigne);
                }
            }
        }
    }

    // fin si window focus id== seq

    ///////////////////AFFICHAGES////////////////////////////////////////////////////

    // Mem excluded from sequence
    /*if(index_is_saving==0 && init_done==1 && index_writing_curve==0 && index_quit==0)
    {

    ExclueMem.MoveTo(Vec2D(x_seq+170,y_seq+140));
    ExclueMem.Draw(CouleurYellow.WithAlpha(alpha_blinker*(MemoiresExclues[position_preset])));
    }*/

    // mem before one
    if (MemoiresExistantes[mem_before_one] == 1) {
        // sprintf(string_mem_before_one,"%d.%d",mem_before_one/10, mem_before_one%10);
        ExclueMem.MoveTo(Vec2D(x_seq + 170, y_seq + 80));
        ExclueMem.Draw(CouleurYellow.WithAlpha(alpha_blinker * (MemoiresExclues[mem_before_one])));

        neuro.Print(string_mem_before_one, x_seq + 115, y_seq + 100);
        // banger
        if (Banger_Memoire[mem_before_one] != 0) // dout
        {
            petitchiffre.Print(ol::ToString(Banger_Memoire[mem_before_one]), x_seq + 220, y_seq + 100);
        }
        // griplayer
        if (set_from_seq_gridplayer1_next_step[mem_before_one] != -1) {
            petitchiffre.Print(ol::ToString(set_from_seq_gridplayer1_next_step[mem_before_one] + 1), x_seq + 251,
                               y_seq + 100);
        }
        if (Times_Memoires[position_onstage][0] > 0.0) {
            petitpetitchiffre.Print(string_time_memonstage[0], x_seq + 20, y_seq + 90);
        }
        if (Times_Memoires[position_onstage][2] > 0.0) // dIn
        {

            petitpetitchiffrerouge.Print(string_time_memonstage[2], x_seq + 20, y_seq + 100);
        }
        // in
        petitpetitchiffre.Print(string_time_memonstage[3], x_seq + 70, y_seq + 90);
        // out
        petitpetitchiffrerouge.Print(string_time_memonstage[1], x_seq + 70, y_seq + 100);
        if (seq_editing_mem == mem_before_one && !seq_editing_annotation)
            seq_draw_editbox(x_seq + 286, y_seq + 90);
        else
            seq_print_clipped(descriptif_memoires[mem_before_one], x_seq + 286, y_seq + 90);
        if (seq_editing_mem == mem_before_one && seq_editing_annotation)
            seq_draw_editbox(x_seq + 286, y_seq + 105);
        else
            seq_print_clipped(annotation_memoires[mem_before_one], x_seq + 286, y_seq + 105);
        if (Links_Memoires[mem_before_one] == 1) {
            Line(Vec2D(x_seq + 187, y_seq + 84), Vec2D(x_seq + 197, y_seq + 84)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 84), Vec2D(x_seq + 197, y_seq + 99)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 99), Vec2D(x_seq + 192, y_seq + 94)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 99), Vec2D(x_seq + 202, y_seq + 94)).Draw(CouleurLigne);
        }
    }

    // mem on stage
    if (MemoiresExistantes[position_onstage] == 1) {
        neuro.Print(string_mem_onstage, x_seq + 115, y_seq + 100 + 30);

        ExclueMem.MoveTo(Vec2D(x_seq + 170, y_seq + 110));
        ExclueMem.Draw(CouleurYellow.WithAlpha(alpha_blinker * (MemoiresExclues[position_onstage])));

        if (seq_editing_mem == position_onstage && !seq_editing_annotation)
            seq_draw_editbox(x_seq + 286, y_seq + 120);
        else
            seq_print_clipped(descriptif_memoires[position_onstage], x_seq + 286, y_seq + 120);
        if (seq_editing_mem == position_onstage && seq_editing_annotation)
            seq_draw_editbox(x_seq + 286, y_seq + 135);
        else
            seq_print_clipped(annotation_memoires[position_onstage], x_seq + 286, y_seq + 135);
        // banger
        if (Banger_Memoire[position_onstage] != 0) {
            petitchiffre.Print(ol::ToString(Banger_Memoire[position_onstage]), x_seq + 220, y_seq + 130);
        }
        // griplayer
        if (set_from_seq_gridplayer1_next_step[position_onstage] != -1) {
            petitchiffre.Print(ol::ToString(set_from_seq_gridplayer1_next_step[position_onstage] + 1), x_seq + 251,
                               y_seq + 130);
        }
        if (Links_Memoires[position_onstage] == 1) {
            Line(Vec2D(x_seq + 187, y_seq + 114 + 5), Vec2D(x_seq + 197, y_seq + 119)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 114 + 5), Vec2D(x_seq + 197, y_seq + 134)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 130 + 4), Vec2D(x_seq + 192, y_seq + 129)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 130 + 4), Vec2D(x_seq + 202, y_seq + 129)).Draw(CouleurLigne);
        }
    }
    // mem on preset
    if (MemoiresExistantes[position_preset] == 1) {
        ExclueMem.MoveTo(Vec2D(x_seq + 170, y_seq + 140));
        ExclueMem.Draw(CouleurYellow.WithAlpha(alpha_blinker * (MemoiresExclues[position_preset])));
        neuro.Print(string_mem_preset, x_seq + 115, y_seq + 160);
        if (seq_editing_mem == position_preset && !seq_editing_annotation)
            seq_draw_editbox(x_seq + 286, y_seq + 150);
        else
            seq_print_clipped(descriptif_memoires[position_preset], x_seq + 286, y_seq + 150);
        if (seq_editing_mem == position_preset && seq_editing_annotation)
            seq_draw_editbox(x_seq + 286, y_seq + 165);
        else
            seq_print_clipped(annotation_memoires[position_preset], x_seq + 286, y_seq + 165);
        petitpetitchiffre.Print(cross_din, x_seq + 20, y_seq + 160);
        petitchiffre.Print(cross_in, x_seq + 55, y_seq + 160);
        petitchiffre.Print(cross_out, x_seq + 55, y_seq + 130);
        petitpetitchiffre.Print(cross_dout, x_seq + 20, y_seq + 130);
        // banger
        if (Banger_Memoire[position_preset] != 0) {
            petitchiffre.Print(ol::ToString(Banger_Memoire[position_preset]), x_seq + 220, y_seq + 160);
        }
        // griplayer
        if (set_from_seq_gridplayer1_next_step[position_preset] != -1) {
            petitchiffre.Print(ol::ToString(set_from_seq_gridplayer1_next_step[position_preset] + 1), x_seq + 251,
                               y_seq + 160);
        }
        if (Links_Memoires[position_preset] == 1) {
            Line(Vec2D(x_seq + 187, y_seq + 149), Vec2D(x_seq + 197, y_seq + 149)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 149), Vec2D(x_seq + 197, y_seq + 164)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 164), Vec2D(x_seq + 192, y_seq + 159)).Draw(CouleurLigne);
            Line(Vec2D(x_seq + 197, y_seq + 164), Vec2D(x_seq + 202, y_seq + 159)).Draw(CouleurLigne);
        }
    }

    // LES MEMOIRES SUIVANTES USER DEFINED
    int index_nbre_mem_visues = 0;
    // sab 03/04/2014 unused var int last_mem_visue=0;
    bool turn = 0;
    for (int memsearch = position_preset + 1; memsearch < 10000; memsearch++) {
        if (memsearch >= 9999 && turn == 0) {
            memsearch = 0;
            turn = 1;
        }
        if (MemoiresExistantes[memsearch] == 1 && index_nbre_mem_visues < nbre_memoires_visualisables_en_preset) {
            index_nbre_mem_visues++;

            // mems
            sprintf(string_next_mem, "%d.%d", memsearch / 10, memsearch % 10);
            neuro.Print(string_next_mem, x_seq + 115, y_seq + 160 + (35 * index_nbre_mem_visues));
            if (seq_editing_mem == memsearch && !seq_editing_annotation)
                seq_draw_editbox(x_seq + 286, y_seq + 150 + (35 * index_nbre_mem_visues));
            else
                seq_print_clipped(descriptif_memoires[memsearch], x_seq + 286,
                                  y_seq + 150 + (35 * index_nbre_mem_visues));
            if (seq_editing_mem == memsearch && seq_editing_annotation)
                seq_draw_editbox(x_seq + 286, y_seq + 165 + (35 * index_nbre_mem_visues));
            else
                seq_print_clipped(annotation_memoires[memsearch], x_seq + 286,
                                  y_seq + 165 + (35 * index_nbre_mem_visues));

            Line(Vec2D(x_seq + 10, y_seq + 100 + 70 + (35 * index_nbre_mem_visues)),
                 Vec2D(x_seq + 450, y_seq + 170 + (35 * index_nbre_mem_visues)))
                .Draw(CouleurLigne.WithAlpha(0.5));

            ExclueMem.MoveTo(Vec2D(x_seq + 170, y_seq + 135 + (35 * index_nbre_mem_visues)));
            ExclueMem.Draw(CouleurYellow.WithAlpha(alpha_blinker * (MemoiresExclues[memsearch])));

            // din dout
            if (Times_Memoires[memsearch][2] > 0.0) {
                petitpetitchiffre.Print(string_time_mem8after[index_nbre_mem_visues][2], x_seq + 20,
                                        y_seq + 150 + (35 * index_nbre_mem_visues));
            }
            if (Times_Memoires[memsearch][0] > 0.0) // dout
            {
                petitpetitchiffrerouge.Print(string_time_mem8after[index_nbre_mem_visues][0], x_seq + 20,
                                             y_seq + 165 + (35 * index_nbre_mem_visues));
            }
            // in out
            // in
            petitpetitchiffre.Print(string_time_mem8after[index_nbre_mem_visues][3], x_seq + 70,
                                    y_seq + 150 + (35 * index_nbre_mem_visues));
            // out
            petitpetitchiffrerouge.Print(string_time_mem8after[index_nbre_mem_visues][1], x_seq + 70,
                                         y_seq + 165 + (35 * index_nbre_mem_visues));
            // banger
            if (Banger_Memoire[memsearch] != 0) {
                petitchiffre.Print(ol::ToString(Banger_Memoire[memsearch]), x_seq + 220,
                                   y_seq + 160 + (35 * index_nbre_mem_visues));
            }
            // griplayer
            if (set_from_seq_gridplayer1_next_step[memsearch] != -1) {
                petitchiffre.Print(ol::ToString(set_from_seq_gridplayer1_next_step[memsearch] + 1), x_seq + 251,
                                   y_seq + 160 + (35 * index_nbre_mem_visues));
            }

            if (Links_Memoires[memsearch] == 1) {
                Line(Vec2D(x_seq + 187, y_seq + 144 + (35 * index_nbre_mem_visues)),
                     Vec2D(x_seq + 197, y_seq + 144 + (35 * index_nbre_mem_visues)))
                    .Draw(CouleurLigne);
                Line(Vec2D(x_seq + 197, y_seq + 144 + (35 * index_nbre_mem_visues)),
                     Vec2D(x_seq + 197, y_seq + 159 + (35 * index_nbre_mem_visues)))
                    .Draw(CouleurLigne);
                Line(Vec2D(x_seq + 197, y_seq + 159 + (35 * index_nbre_mem_visues)),
                     Vec2D(x_seq + 192, y_seq + 154 + (35 * index_nbre_mem_visues)))
                    .Draw(CouleurLigne);
                Line(Vec2D(x_seq + 197, y_seq + 159 + (35 * index_nbre_mem_visues)),
                     Vec2D(x_seq + 202, y_seq + 154 + (35 * index_nbre_mem_visues)))
                    .Draw(CouleurLigne);
            }

            if (index_nbre_mem_visues >= nbre_memoires_visualisables_en_preset) {
                break;
            }
        }
    }

    return (0);
}

int x1_x2(int x_seq, int y_seq) {
    Rect FaderX1(Vec2D(x_seq + 480, y_seq + 80), Vec2D(45, 255)); // box du fader
    FaderX1.SetRoundness(15);
    FaderX1.SetLineWidth(epaisseur_ligne_fader);
    Rect FaderNiveauX1(Vec2D(x_seq + 480, ((y_seq + 335) - niveauX1)), Vec2D(45, niveauX1 + 2)); // niveau fader
    FaderNiveauX1.SetRoundness(15);
    Rect FaderX2(Vec2D(x_seq + 580, y_seq + 80), Vec2D(45, 255)); // box du fader
    FaderX2.SetRoundness(15);
    FaderX2.SetLineWidth(epaisseur_ligne_fader);
    Rect FaderNiveauX2(Vec2D(x_seq + 580, ((y_seq + 335 - niveauX2))), Vec2D(45, niveauX2)); // niveau fader
    FaderNiveauX2.SetRoundness(15);
    FaderNiveauX1.Draw(CouleurNiveau.WithAlpha(0.5));
    FaderX1.DrawOutline(CouleurLigne);
    FaderNiveauX2.Draw(CouleurSurvol);
    FaderX2.DrawOutline(CouleurLigne);

    raccrochage_midi_visuel_vertical_dmx(x_seq + 480, (y_seq + 80), 491, 45, 255);
    raccrochage_midi_visuel_vertical_dmx_inverted(x_seq + 580, (y_seq + 80), 492, 45, 255);
    /////////////////////////////////////CROSSFADE MANUEL/////////////////////////////

    /////////////////////////////////////////////////////////////////
    if (index_go == 1 || index_pause == 1) {
        if (crossfade_time_delay_out > (actual_time - crossfade_start_time)) {
            FaderX1.Draw(CouleurBlind.WithAlpha(alpha_blinker));
        }
        if (crossfade_time_delay_in > (actual_time - crossfade_start_time)) {
            FaderX2.Draw(CouleurBlind.WithAlpha(alpha_blinker));
        }
    }
    if (dmx_view == 1) {
        neuro.Print(ol::ToString(niveauX1), x_seq + 485, y_seq + 70);
        neuro.Print(ol::ToString(niveauX2), x_seq + 590, y_seq + 70);
    } else {
        neuro.Print(ol::ToString((int)((float)niveauX1 / 2.55)), x_seq + 485, y_seq + 70);
        neuro.Print(ol::ToString((int)((float)niveauX2 / 2.55)), x_seq + 590, y_seq + 70);
    }

    Rect X1X2Together(Vec2D(x_seq + 530, y_seq + 50), Vec2D(50, 20)); // box du fader
    X1X2Together.SetRoundness(7.5);
    X1X2Together.SetLineWidth(demi_epaisseur_ligne_fader);
    X1X2Together.Draw(CouleurFader.WithAlpha(index_x1_x2_together));
    X1X2Together.DrawOutline(CouleurLigne);

    Line(Vec2D(x_seq + 540, y_seq + 55), Vec2D(x_seq + 550, y_seq + 55)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 550, y_seq + 55), Vec2D(x_seq + 560, y_seq + 65)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 560, y_seq + 65), Vec2D(x_seq + 570, y_seq + 65)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 540, y_seq + 65), Vec2D(x_seq + 550, y_seq + 65)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 550, y_seq + 65), Vec2D(x_seq + 560, y_seq + 55)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 560, y_seq + 55), Vec2D(x_seq + 570, y_seq + 55)).Draw(CouleurLigne);
    // clamp défensif rendu
    int r_display = ratio_X1X2_together;
    if(r_display > 0)    r_display = 0;
    if(r_display < -255) r_display = -255;
    Rect index_together(Vec2D(x_seq + 535, y_seq + 70 + r_display + 255), Vec2D(40, 20)); // curseur: ratio=-255→gorge-top, ratio=0→gorge-bottom
    index_together.SetRoundness(4);
    index_together.SetLineWidth(epaisseur_ligne_fader);

    Line(Vec2D(x_seq + 555, y_seq + 70), Vec2D(x_seq + 555, y_seq + 70 + 255)).Draw(CouleurLigne);       // gorge
    Line(Vec2D(x_seq + 525, y_seq + 90), Vec2D(x_seq + 555, y_seq + 90)).Draw(CouleurLigne);             // vers stage
    Line(Vec2D(x_seq + 555, y_seq + 70 + 255), Vec2D(x_seq + 580, y_seq + 70 + 255)).Draw(CouleurLigne); // vers stage
    index_together.Draw(CouleurFond);
    index_together.DrawOutline(CouleurLigne);

    petitchiffre.Print(string_ratio_x1x2, x_seq + 540, y_seq + 340 + r_display); // label 15px sous curseur (curseur=y_seq+325+r)

    Circle GoTouch(Vec2D(x_seq + 540 + 14, y_seq + 375), 20);
    GoTouch.SetLineWidth(2);
    GoTouch.DrawOutline(CouleurLigne);
    GoTouch.DrawOutline(CouleurGreen.WithAlpha(Midi_Force_Go));

    GoTouch.Draw(CouleurFader.WithAlpha(index_go));
    GoTouch.DrawOutline(CouleurLigne);
    GoTouch.Draw(CouleurFond.WithAlpha(alpha_blinker * index_pause));
    GoTouch.DrawOutline(CouleurLigne);

    Line(Vec2D(x_seq + 546, y_seq + 365), Vec2D(x_seq + 566, y_seq + 375)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 546, y_seq + 385), Vec2D(x_seq + 566, y_seq + 375)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 546, y_seq + 365), Vec2D(x_seq + 546, y_seq + 385)).Draw(CouleurLigne);

    //////////////go back
    Circle GoBackTouch(Vec2D(x_seq + 504, y_seq + 375), 20);
    GoBackTouch.SetLineWidth(2);
    GoBackTouch.DrawOutline(CouleurLigne);
    GoBackTouch.Draw(CouleurFader.WithAlpha(index_go_back));
    GoBackTouch.DrawOutline(CouleurLigne);
    GoBackTouch.Draw(CouleurFond.WithAlpha(alpha_blinker * index_pause));
    GoBackTouch.DrawOutline(CouleurLigne);

    Line(Vec2D(x_seq + 512, y_seq + 365), Vec2D(x_seq + 492, y_seq + 375)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 512, y_seq + 385), Vec2D(x_seq + 492, y_seq + 375)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 512, y_seq + 365), Vec2D(x_seq + 512, y_seq + 385)).Draw(CouleurLigne);

    //////////////double go
    Circle DoubleGoTouch(Vec2D(x_seq + 604, y_seq + 375), 20);
    DoubleGoTouch.SetLineWidth(2);
    DoubleGoTouch.DrawOutline(CouleurLigne);

    Line(Vec2D(x_seq + 593, y_seq + 365), Vec2D(x_seq + 613, y_seq + 375)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 593, y_seq + 385), Vec2D(x_seq + 613, y_seq + 375)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 593, y_seq + 365), Vec2D(x_seq + 593, y_seq + 385)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 602, y_seq + 365), Vec2D(x_seq + 620, y_seq + 375)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 602, y_seq + 385), Vec2D(x_seq + 620, y_seq + 375)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 602, y_seq + 365), Vec2D(x_seq + 602, y_seq + 368)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 602, y_seq + 385), Vec2D(x_seq + 602, y_seq + 382)).Draw(CouleurLigne);

    /////////////////////ACCELEROMETRE

    // Cadre bouton speed
    Rect SequencielSpeedFrame(Vec2D(x_seq + 485, y_seq + 405), Vec2D(127 + 10, 22));
    SequencielSpeedFrame.SetLineWidth(epaisseur_ligne_fader);
    SequencielSpeedFrame.SetRoundness(5);
    SequencielSpeedFrame.DrawOutline(CouleurLigne);
    Rect SequencielSpeed(Vec2D(x_seq + 485 + crossfade_speed, y_seq + 405), Vec2D(10, 22));
    SequencielSpeed.SetLineWidth(tiers_epaisseur_ligne_fader);
    SequencielSpeed.SetRoundness(5);

    Rect OnLinkReset(Vec2D(x_seq + 548, y_seq + 435), Vec2D(15, 15));
    OnLinkReset.Draw(CouleurBlind.WithAlpha(auto_reset_crossfade_speed_on_link));
    OnLinkReset.DrawOutline(CouleurLigne);

    Line(Vec2D(x_seq + 554, y_seq + 405), Vec2D(x_seq + 554, y_seq + 427)).Draw(CouleurLigne);

    petitchiffre.Print(str_crossfade_speed_is, x_seq + 465, y_seq + 430);
    SequencielSpeed.Draw(CouleurFader);

    // midi out des x1 et x2
    Circle BMidiOut(x_seq + 655, y_seq + 340, 10); // box du fader
    BMidiOut.SetLineWidth(epaisseur_ligne_fader);
    BMidiOut.Draw(CouleurBlind.WithAlpha(midi_send_out[491]));
    BMidiOut.DrawOutline(CouleurLigne);
    // liaison visuelle au fader du cercle midi out
    Line(Vec2D(x_seq + 620, y_seq + 330), Vec2D(x_seq + 640, y_seq + 330)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 640, y_seq + 330), Vec2D(x_seq + 647, y_seq + 334)).Draw(CouleurLigne);

    // MIDI OUT DU SPEED
    Circle BSpeedMidiOut(x_seq + 655, y_seq + 425, 10); // box du fader
    BSpeedMidiOut.SetLineWidth(epaisseur_ligne_fader);
    BSpeedMidiOut.Draw(CouleurBlind.WithAlpha(midi_send_out[493]));
    BSpeedMidiOut.DrawOutline(CouleurLigne);
    // liaison visuelle au fader du cercle midi out
    Line(Vec2D(x_seq + 620, y_seq + 415), Vec2D(x_seq + 640, y_seq + 415)).Draw(CouleurLigne);
    Line(Vec2D(x_seq + 640, y_seq + 415), Vec2D(x_seq + 647, y_seq + 419)).Draw(CouleurLigne);

    raccrochage_midi_visuel_horizontal(x_seq + 490, y_seq + 405, 493, 127, 13);

    // OVER MOUSE

    if (window_focus_id == W_SEQUENCIEL) {

        if (Midi_Faders_Affectation_Type != 0) {
            // X1 X2
            if (mouse_y >= y_seq + 60 && mouse_y <= y_seq + 355) //-20 +20
            {
                if (mouse_x > x_seq + 480 && mouse_x < x_seq + 525) {
                    FaderX1.DrawOutline(CouleurBlind);
                } else if (mouse_x > x_seq + 580 && mouse_x < x_seq + 625) {
                    FaderX2.DrawOutline(CouleurBlind);
                }
            } else if (mouse_x > x_seq + 534 && mouse_x < x_seq + 574 && mouse_y > y_seq + 355 &&
                       mouse_y < y_seq + 395) {
                GoTouch.DrawOutline(CouleurBlind);
            } else if (mouse_x > x_seq + 484 && mouse_x < x_seq + 524 && mouse_y > y_seq + 355 &&
                       mouse_y < y_seq + 395) {
                GoBackTouch.DrawOutline(CouleurBlind);
            } else if (mouse_x > x_seq + 584 && mouse_x < x_seq + 624 && mouse_y > y_seq + 355 &&
                       mouse_y < y_seq + 395) {
                DoubleGoTouch.DrawOutline(CouleurBlind);
            } else if (mouse_x > x_seq + 548 && mouse_x < x_seq + 563 && mouse_y > y_seq + 435 &&
                       mouse_y < y_seq + 450 && Midi_Faders_Affectation_Type != 0) {
                char sttmp[24];
                sprintf(sttmp, "AutoReset Xfade Speed");
                show_type_midi(1591, sttmp);
                OnLinkReset.DrawOutline(CouleurBlind);
            }
            // Accelerometre
            else if (mouse_x >= x_seq + 490 && mouse_x <= x_seq + 617 && mouse_y > y_seq + 405 &&
                     mouse_y < y_seq + 427) {
                // midi report
                switch (miditable[0][493]) {
                // midi report
                case 0:
                    sprintf(thetypinfo, "Note");
                    break;
                case 1:
                    sprintf(thetypinfo, "Key On");
                    break;
                case 2:
                    sprintf(thetypinfo, "Key Off");
                    break;
                case 4:
                    sprintf(thetypinfo, "Ctrl Change");
                    break;
                }
                sprintf(string_last_midi_id, "Sequenciel SPEED is Ch: %d Pitch: %d Typ: %s", miditable[1][493],
                        miditable[2][493], thetypinfo);
                SequencielSpeedFrame.DrawOutline(CouleurBlind);
            }
        }

        // fin focus window
    }
    return (0);
}

int Sequenciel_Window(int xseq, int yseq, int largeur_seq, int hauteur_seq) {
    // background window
    Rect SeqWindow(Vec2D(xseq, yseq), Vec2D(largeur_seq, hauteur_seq));
    SeqWindow.SetRoundness(15);
    SeqWindow.SetLineWidth(epaisseur_bordure_fenetre);
    SeqWindow.Draw(CouleurFond);
    if (window_focus_id == W_SEQUENCIEL) {
        SeqWindow.DrawOutline(CouleurFader);
    } else {
        SeqWindow.DrawOutline(CouleurLigne);
    }
    neuro.Print("CUELIST", xseq + 90, yseq + 30);

    /////////////////////////////TITRES////////////////////////////////////////////

    petitpetitchiffre.Print("Delay", xseq + 20, yseq + 60);
    petitpetitchiffre.Print("In/Out", xseq + 60, yseq + 60);
    neuromoyen.Print("Mem", xseq + 115, yseq + 60);

    Rect InPositionStage(Vec2D(xseq + 10, yseq + 80), Vec2D(100, 40));
    InPositionStage.SetRoundness(4);
    InPositionStage.Draw(CouleurNiveau.WithAlpha(0.5));

    Rect PositionStage(Vec2D(xseq + 10, yseq + 110), Vec2D(460, 40));
    PositionStage.Draw(CouleurNiveau.WithAlpha(0.5));

    Rect OutPositionStage(Vec2D(xseq + 10, yseq + 110), Vec2D(100, 40));
    OutPositionStage.SetRoundness(4);
    OutPositionStage.Draw(CouleurSurvol);

    Rect PositionPreset(Vec2D(xseq + 10, yseq + 140), Vec2D(460, 30));
    PositionPreset.SetRoundness(7.5);
    PositionPreset.Draw(CouleurSurvol);

    ///////////////////////////////////////////////////////////////////////////////

    refresh_vision_memories(xseq, yseq);
    x1_x2(xseq, yseq - 25);

    ////////////////////MENUS///////////////////////////////////////////////////////
    // options hautes
    // LINK
    Rect SeqModL(Vec2D(xseq + 182, yseq + 50), Vec2D(30, 15));
    SeqModL.Draw(CouleurFader.WithAlpha(index_link_is_on));
    SeqModL.DrawOutline(CouleurLigne.WithAlpha(0.5));
    petitchiffre.Print("Link", xseq + 185, yseq + 60);
    // BANGER
    Rect SeqMod(Vec2D(xseq + 215, yseq + 50), Vec2D(30, 15));
    SeqMod.Draw(CouleurFader.WithAlpha(index_banger_is_on));
    SeqMod.DrawOutline(CouleurLigne.WithAlpha(0.5));
    petitchiffre.Print("Bang", xseq + 215, yseq + 60);
    // GRID PLAYER EMBEDED
    Rect GplMod(Vec2D(xseq + 248, yseq + 50), Vec2D(30, 15));
    GplMod.Draw(CouleurFader.WithAlpha(show_gridplayer_in_seq));
    GplMod.DrawOutline(CouleurLigne.WithAlpha(0.5));
    petitchiffre.Print("Gpl.1", xseq + 250, yseq + 60);

    /////menus bas
    // sab 02/03/2014 unused var char string_seq_function[5];
    Rect SeqAction(Vec2D(xseq, yseq), Vec2D(50, 20));
    SeqAction.SetRoundness(7.5);

    for (int op = 0; op < 7; op++) {
        // Rect SeqAction(Vec2D (xseq+20+(op*60), yseq+395 ), Vec2D ( 50,20));
        int rangee_y = yseq + 180 + (35 * (nbre_memoires_visualisables_en_preset + 1));

        SeqAction.MoveTo(Vec2D(xseq + 20 + (op * 60), rangee_y));
        // affichage
        SeqAction.DrawOutline(CouleurLigne);
        if (window_focus_id == W_SEQUENCIEL && Midi_Faders_Affectation_Type != 0 && mouse_x > xseq + 20 + (op * 60) &&
            mouse_x < xseq + 70 + (op * 60) && mouse_y > rangee_y && mouse_y < rangee_y + 20) // config midi
        {
            SeqAction.DrawOutline(CouleurBlind);
        }
        switch (op) {
        case 0:
            petitchiffre.Print("Create", xseq + 23 + (op * 60), rangee_y + 15);
            if (Midi_Faders_Affectation_Type != 0)
                show_type_midi(768, "CREATE MEM");
            break;
        case 1:
            petitchiffre.Print("Delete", xseq + 23 + (op * 60), rangee_y + 15);
            if (Midi_Faders_Affectation_Type != 0)
                show_type_midi(769, "DELETE MEM");
            break;
        case 2:
            petitchiffre.Print("Stage-", xseq + 23 + (op * 60), rangee_y + 15);
            if (Midi_Faders_Affectation_Type != 0)
                show_type_midi(770, "STAGE MINUS");
            break;
        case 3:
            petitchiffre.Print("Stage+", xseq + 23 + (op * 60), rangee_y + 15);
            if (Midi_Faders_Affectation_Type != 0)
                show_type_midi(771, "STAGE PLUS");
            break;
        case 4:
            petitchiffre.Print("Preset-", xseq + 23 + (op * 60), rangee_y + 15);
            if (Midi_Faders_Affectation_Type != 0)
                show_type_midi(772, "PRESET MINUS");
            break;
        case 5:
            petitchiffre.Print("Preset+", xseq + 23 + (op * 60), rangee_y + 15);
            if (Midi_Faders_Affectation_Type != 0)
                show_type_midi(773, "PRESET PLUS");
            break;
        case 6:
            petitchiffre.Print("GOTO", xseq + 27 + (op * 60), rangee_y + 15);
            if (Midi_Faders_Affectation_Type != 0)
                show_type_midi(1645, "GOTO");
            break;
        default:
            break;
        }
    }

    /////////////////AVERTISSEMENT GET BACK MIDI FADERS
    if (index_get_back_faders_need_to_be_done == 1) {
        Rect FondClignotant(Vec2D(xseq + 450, yseq + 185), Vec2D(200, 30));
        FondClignotant.SetRoundness(15);
        FondClignotant.Draw(CouleurFond);
        FondClignotant.Draw(CouleurBlind.WithAlpha(alpha_blinker));

        petitdoomInspekt.Print("GET UP MIDI FADERS !", xseq + 470, yseq + 205);
    }

    // TIME LEFT

    neuro.Print(string_time_left_is, xseq + 380, yseq + 30);
    neuromoyen.Print("---", xseq + 430, yseq + 40);
    neuro.Print(string_total_time_is, xseq + 380, yseq + 60);
    // colonnages
    Line(Vec2D(xseq + 170, yseq + 80), Vec2D(xseq + 170, yseq + hauteur_seq - 75))
        .Draw(CouleurLigne.WithAlpha(0.5)); // après mem
    Line(Vec2D(xseq + 180, yseq + 80), Vec2D(xseq + 180, yseq + hauteur_seq - 75))
        .Draw(CouleurLigne.WithAlpha(0.5)); // après exclude mem
    Line(Vec2D(xseq + 213, yseq + 80), Vec2D(xseq + 213, yseq + hauteur_seq - 75))
        .Draw(CouleurLigne.WithAlpha(0.5)); // apres link
    Line(Vec2D(xseq + 246, yseq + 80), Vec2D(xseq + 246, yseq + hauteur_seq - 75))
        .Draw(CouleurLigne.WithAlpha(0.5)); // apres banger
    Line(Vec2D(xseq + 280, yseq + 80), Vec2D(xseq + 280, yseq + hauteur_seq - 75))
        .Draw(CouleurLigne.WithAlpha(0.5)); // apres gpl.1

    // Grid Player 1 enchassé
    if (show_gridplayer_in_seq == 1) {
        neuromoyen.Print("GridPlayer 1 /", xseq + 660, yseq + 40);
        neuromoyen.Print("Grid ", xseq + 760, yseq + 40);
        neuromoyen.Print(ol::ToString(index_grider_selected[0] + 1), xseq + 810, yseq + 40);
        Line(Vec2D(xseq + 660, yseq + 50), Vec2D(xseq + 860, yseq + 50)).Draw(CouleurLigne);

        // UP DOWN griders number selected
        Circle GriderPlus(Vec2D(xseq + 910, yseq + 40), 12);
        Circle GriderMinus(Vec2D(xseq + 880, yseq + 40), 12);

        petitchiffre.Print("-", xseq + 877, yseq + 44);
        petitchiffre.Print("+", xseq + 907, yseq + 44);

        GriderPlus.DrawOutline(CouleurLigne);
        GriderMinus.DrawOutline(CouleurLigne);

        if (window_focus_id == W_SEQUENCIEL) {

            if (mouse_x > xseq + 880 - 12 && mouse_x < xseq + 892 && mouse_y > yseq + 28 && mouse_y < yseq + 52) {
                if (Midi_Faders_Affectation_Type != 0) {
                    char sttmp[24];
                    sprintf(sttmp, "Minus Grid in GridPl.1");
                    show_type_midi(1476, sttmp);
                    GriderMinus.DrawOutline(CouleurBlind);
                }
            }

            if (mouse_x > xseq + 898 && mouse_x < xseq + 922 && mouse_y > yseq + 28 && mouse_y < yseq + 52) {
                if (Midi_Faders_Affectation_Type != 0) {
                    char sttmp[24];
                    sprintf(sttmp, "Plus Grid in GridPl.1");
                    show_type_midi(1480, sttmp);
                    GriderPlus.DrawOutline(CouleurBlind);
                }
            }
        }
        petitchiffre.Print(grider_name[index_grider_selected[0]], xseq + 660, yseq + 70);
        TheStepBox(xseq + 660, yseq + 90, 0);
        ThePlayCommands(xseq + 760, yseq + 90, 0);
        TheGrid_commands(xseq + 660, yseq + 130, 0); // pos x, posy y, num player

        Rect BoxLink(Vec2D(xseq + 690, yseq + 295), Vec2D(105, 15));
        BoxLink.Draw(CouleurFader.WithAlpha(index_link_speed_crossfade_to_gpl1));
        BoxLink.DrawOutline(CouleurLigne.WithAlpha(0.5));
        petitchiffre.Print("Speed is CueList", xseq + 693, yseq + 305);
        if (window_focus_id == W_SEQUENCIEL && mouse_x > xseq + 690 && mouse_x < xseq + 795 && mouse_y > yseq + 295 &&
            mouse_y < yseq + 310) {
            if (Midi_Faders_Affectation_Type != 0) {
                char sttmp[24];
                sprintf(sttmp, "Speed is CueList GPl.1");
                show_type_midi(1540, sttmp);
                BoxLink.DrawOutline(CouleurBlind);
            }
        }
    }

    return (0);
}
