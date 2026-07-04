#pragma once

// -----------------------------------------------------------------------------
// Editeur de texte inline reutilisable (double-clic dans un champ -> saisie directe,
// sans passer par la fenetre texte F5).
//
// Reutilise l'infra de saisie existante : confirm_name_buf / confirm_name_len /
// seq_edit_cursor / index_confirm_name_active (curseur, backspace, fleches, UTF-8,
// SDL_TextInput deja cables dans keyboard_routines2). Une seule session active a la fois.
//
// But : ajouter l'edition inline a un nouveau champ SANS dupliquer de code ni ajouter
// de flag/branche clavier. Cote fenetre, il suffit de trois points d'accroche :
//
//   // au double-clic sur le champ (cible = buffer a editer) :
//   if (mouse_double_click) wc_inline_begin(mon_buffer, sizeof(mon_buffer), 225, ma_callback);
//
//   // au rendu du champ :
//   if (wc_inline_editing(mon_buffer)) wc_inline_render(x, y, caret_top, caret_bot);
//   else                               petitchiffre.Print(mon_buffer, x, y);
//
//   // sur le bouton d'action (valide le nom en cours avant d'agir) :
//   if (wc_inline_active()) wc_inline_commit();
//
// Enter valide (commit), Echap annule (cancel) : gere globalement dans keyboard_routines2.
// on_commit (optionnel) est appelee APRES l'ecriture de la cible (ex. re-detecter un format).
// -----------------------------------------------------------------------------

void wc_inline_begin(char* target, int target_cap, int max_px, void (*on_commit)());
void wc_inline_commit();   // ecrit confirm_name_buf -> cible, appelle on_commit, ferme la session
void wc_inline_cancel();   // ferme la session sans ecrire la cible
bool wc_inline_active();   // true si une session inline generique est en cours
bool wc_inline_editing(const char* target); // ... et sa cible est 'target'
int  wc_inline_max_px();   // limite de largeur pixel de la session courante (pour le clavier)
void wc_inline_render(int x, int y, int caret_top, int caret_bot); // texte en cours + caret blanc
