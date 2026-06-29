#include <SmoothData.h>
// max_damper_mode defini dans SmoothData.h

SmoothData::SmoothData()
{
    //damper
    _damper_do_calculation=0;
    _damper_mode=0;
    _damper_decay_constant = 1.0; // [fader 16 bit] defaut plus vif (etait 0.6)
    _damper_vel = 0.1;
    _damper_dt = 0.3; // [fader 16 bit] defaut plus vif (etait 0.1, plage dt desormais 0..1) -> ~0.4s
    _damper_val = 0.0;
    _damper_target_val = 0.0;
    _damper_previous_target=0.0;
    _damper_blocking_mode = false; // [fader 16 bit] etait non initialise -> pouvait figer le damper
    _damper_accel = 0.0;
}

////////////////////////////////////////////////////
float SmoothData::getdecay()
{
    return(_damper_decay_constant);
}

float SmoothData::getdt()
{
    return(_damper_dt);
}


int SmoothData::getdampermode()
{
    return(_damper_mode);
}

void  SmoothData::set_value(int value)
{
    _data_val=value;
    if(_data_val<0) _data_val=0;
    else if(_data_val>65535) _data_val=65535; // [fader 16 bit]
}

////////////////////////////////////////////////////
void SmoothData::set_damper_decay(float value)
{
 _damper_decay_constant=value;
 if (_damper_decay_constant<.01) _damper_decay_constant=.01;
 if (_damper_decay_constant>1.) _damper_decay_constant=1.;
}

void SmoothData::set_damper_dt(float value)
{
 _damper_dt=value;
 if (_damper_dt<.001) _damper_dt=.001;
 if(_damper_dt> DAMPER_DT_MAX) _damper_dt= DAMPER_DT_MAX; // [fader 16 bit] plage dt 0..DAMPER_DT_MAX (etait 0.1)
}

void SmoothData::set_damper_mode(int mode)
{
    _damper_mode=mode;
    if(_damper_mode>max_damper_mode){_damper_mode=0;}
}


void SmoothData::set_damper_blocking_mode(bool state)
{
    _damper_blocking_mode=state;
}

void SmoothData::set_target_val(int val)
{
    _data_val=val;
}

bool SmoothData::calculating()//to know if calculation is done
{
    return(_damper_do_calculation);
}

void SmoothData::fix_all_damper_state_value(int val)
{
    _data_val=val;
    _damper_target_val=((float)(_data_val))/65535; // [fader 16 bit]
    _damper_previous_target= _damper_target_val;
    _damper_val=_damper_target_val;

}

void SmoothData::damper()
{
_damper_previous_target=_damper_target_val;

float tmp=_data_val;
_damper_target_val=((float)(_data_val))/65535; // [fader 16 bit]


if (!_damper_blocking_mode)
{
    _damper_do_calculation=1;
}
else
{
    _damper_do_calculation=0;
    if(tmp!=_damper_previous_target) {_damper_do_calculation=1;}
}

if(_damper_do_calculation==1)
{
    switch (_damper_mode)
    {
    /*case 1://elastic effect
    _damper_accel = (_damper_target_val - _damper_val) * 1 - _damper_vel *  _damper_decay_constant;
    _damper_vel += _damper_accel * _damper_dt;
    _damper_val += _damper_vel * _damper_dt;*/
    case 0://plus veloce
    _damper_vel = (_damper_target_val - _damper_val) * _damper_decay_constant;
    _damper_val += _damper_vel * _damper_dt;
    break;
    break;
    case 1://moins veloce
    _damper_vel= ((_damper_target_val-_damper_val)* _damper_decay_constant)/2;
    _damper_val+=(_damper_vel*_damper_dt)* _damper_decay_constant;
    break;
    case 2://in out egaux
    _damper_vel= (_damper_target_val-_damper_val)* _damper_decay_constant;
    _damper_val+=(_damper_vel*_damper_dt)* _damper_decay_constant;
    break;

    default:
    break;
    }
    if(_damper_val<0.0){_damper_val=0.0; _damper_do_calculation=0;}
    else if(_damper_val>1.0){_damper_val=1.0; _damper_do_calculation=0;}
    // [fader 16 bit] snap : la glisse exponentielle est asymptotique (n'atteint jamais la cible pile).
    // On accroche exactement la cible (0 / plein / valeur) quand l'ecart vaut moins d'1 unite 12 bit
    // (1/4096, soit ~16 unites 16 bit) : raccourcit la traine tout en gardant une bonne precision.
    // Ce seuil reste bien plus fin qu'un pas MIDI 7 bit -> le lissage MIDI 16 bit n'est pas affecte.
    float _ecart = _damper_target_val - _damper_val;
    if (_ecart < 0.0f) _ecart = -_ecart;
    if (_ecart * 4096.0f < 1.0f) { _damper_val = _damper_target_val; _damper_do_calculation = 0; }
}


_dampered_data=_damper_val*65535; // [fader 16 bit]
}

float SmoothData::getvalue_dampered()
{
     return(_damper_val);
}

int SmoothData::getdmxvalue_dampered()
{
    return(_dampered_data);
}


int SmoothData::getdmxvalue()
{
int val=_data_val*255;
return val;
}

////////////////////////////////////////////////////
