/************************************************************************************************************
 * SmoothData.h -
 Christoph Guillermet
 http://le-chat-noir-numerique.fr
 22/04/2015
 ************************************************************************************************************/

#ifndef SmoothData_h
#define SmoothData_h


class SmoothData {
  public:
    SmoothData ();
    void set_value(int value);//pour passer une donn�e � damperiser, venant de la souris, du midi ou arduino ou icat
    void damper();//traitement de la donn�e en damper
    void set_target_val(int val);//specifies la valeur d'arriv�e
    void fix_all_damper_state_value(int val);//raccorde la valeur si elle est manipul�e hors mode damper
    //damper specifique
    void set_damper_decay(float value);
    void set_damper_dt(float value);
    void set_damper_mode(int mode);
    void set_damper_blocking_mode(bool state);


    int getdmxvalue();//renvoi de la valeur en dmx raw_data
    int getdmxvalue_dampered();//0 � 255
    float getvalue_dampered();// 0.0 � 1.0
    float getdecay();
    float getdt();
    int getdampermode();
    bool calculating();//to know if calculation is actually in process

  private:

    int _data_val;//valeur du capteur apr�s process
    //damper
    bool _damper_do_calculation;
    bool _damper_blocking_mode;//sortie de la main bloque la donn�e au niveau
    int _damper_mode;//0 default , 1 montee plus rapide, descente plus lente
    int _dampered_data;
    float _damper_decay_constant;
    float _damper_vel;
    float _damper_dt;
    float _damper_val; //value
    float _damper_target_val;
    float _damper_previous_target;
    float _damper_accel;

};

const int max_damper_mode = 2;

// [fader 16 bit] plage max du Delta (dt) du damper : Delta 127 (curseur plein) = DAMPER_DT_MAX.
// dt eleve = glisse rapide (settle court) ; bas = lisse/lent. Regle ici la vitesse max du damper.
const float DAMPER_DT_MAX = 0.4f;

#endif
