#include "wc_tus.h"
#include "wc_inline_edit.h"

// Session inline courante (une seule a la fois). Le TEXTE et le curseur vivent dans les globals
// partages confirm_name_buf / confirm_name_len / seq_edit_cursor ; ici on ne garde que la CIBLE
// (ou ecrire au commit), sa capacite, la largeur du champ et le callback optionnel.
static char* s_target = 0;
static int   s_cap = 0;
static int   s_max_px = 225;
static void (*s_on_commit)() = 0;

void wc_inline_begin(char* target, int target_cap, int max_px, void (*on_commit)())
{
    if (!target) return;
    s_target = target; s_cap = target_cap; s_max_px = max_px; s_on_commit = on_commit;

    // charge la valeur actuelle dans le buffer d'edition (on edite, on ne repart pas de zero)
    int n = (int)strlen(target);
    if (n > 49) n = 49;
    memcpy(confirm_name_buf, target, n);
    confirm_name_buf[n] = '\0';
    confirm_name_len = n;
    seq_edit_cursor  = n;
    seq_editing_mem  = -1;   // cible = ce champ, pas une memoire du sequenciel
    index_confirm_name_active = 1;
    SDL_StartTextInput();
    wc_dirty = true;
}

static void wc_inline_reset()
{
    s_target = 0; s_cap = 0; s_on_commit = 0;
    confirm_name_buf[0] = '\0';
    confirm_name_len = 0;
    seq_edit_cursor  = 0;
    index_confirm_name_active = 0;
    SDL_StopTextInput();
    wc_dirty = true;
}

void wc_inline_commit()
{
    if (!s_target) return;
    int n = s_cap - 1;
    if (n > 49) n = 49;      // confirm_name_buf fait 50
    if (n < 0)  n = 0;
    strncpy(s_target, confirm_name_buf, n);
    s_target[n] = '\0';
    void (*cb)() = s_on_commit;
    wc_inline_reset();
    if (cb) cb();            // ex. re-detecter le format d'import d'apres l'extension
}

void wc_inline_cancel()
{
    wc_inline_reset();
}

bool wc_inline_active()
{
    return index_confirm_name_active && s_target != 0;
}

bool wc_inline_editing(const char* target)
{
    return wc_inline_active() && s_target == target;
}

int wc_inline_max_px()
{
    return s_max_px;
}

void wc_inline_render(int x, int y, int caret_top, int caret_bot)
{
    petitchiffre.Print(confirm_name_buf, x, y);
    // caret blanc a la position du curseur (largeur du texte jusqu'au curseur)
    char cb[50];
    int n = seq_edit_cursor;
    if (n < 0)  n = 0;
    if (n > 49) n = 49;
    memcpy(cb, confirm_name_buf, n);
    cb[n] = '\0';
    int cx = x + petitchiffre.TextWidth(cb);
    if (alpha_blinker > 0.5f) {
        Line(Vec2D(cx, caret_top), Vec2D(cx, caret_bot)).Draw(CouleurBlanc);
    }
}
