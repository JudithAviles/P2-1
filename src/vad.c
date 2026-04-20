#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N, float fm) {
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  feat.p = compute_power(x,N);
  feat.zcr = compute_zcr(x,N,fm);
  feat.am = compute_am(x,N);
  //feat.zcr = feat.am = (float) rand()/RAND_MAX;
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate, float beta) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->init_count = 0;
  vad_data->accumulated_p = 0.0f;
  vad_data->beta = beta; 
  vad_data->llindar0 = 0.0f; 
  vad_data->llindar_zcr = 0.0f;

  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  VAD_STATE state = vad_data->state;
  if (state == ST_UNDEF) state = ST_SILENCE;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}



VAD_STATE vad(VAD_DATA *vad_data, float *x, float alpha0, float alpha1) {

  //Implementar preprocesado de señal para eliminar ruido
  //    - data cleaning
  //       librosa -> libreria python
  //       import noisereduce as nr
  //       y, sr = librosa.load(audio_path, sr=None)
  //       cleaned_audio = nr.reduce_noise(y=y, sr=sr)
  //Llindars -> potencia mas pequeña que minimo mas duracion.

  Features f = compute_features(x, vad_data->frame_length , vad_data->sampling_rate);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  switch (vad_data->state) {
  case ST_INIT:
    vad_data->accumulated_p += f.p;
    vad_data->init_count++;
    
    if(vad_data->init_count >= 10) {
      float mean_p = vad_data->accumulated_p / 10.0f;
      vad_data->llindar0 = mean_p + alpha0;
      vad_data->llindar_zcr = alpha1 * vad_data->sampling_rate / 2.0f;
      vad_data->state = ST_SILENCE;
      vad_data->hangover = 0;
    }
    return ST_SILENCE;
    break;

  case ST_SILENCE: {
    int may_be_voice = (f.p > vad_data->llindar0 || f.zcr > vad_data->llindar_zcr); // Maybe put zcr < llindar? El soroll té zcr més alt que la veu generalment
    if (may_be_voice) {
      vad_data->state = ST_VOICE;
      vad_data->hangover = 7;
    } else {
      // Threshold adaptatiu:
      // Formula: llindar0 = (1-beta)*(llindar0-alpha0) + beta*P + alpha0
      vad_data->llindar0 = (1 - vad_data->beta) * (vad_data->llindar0 - alpha0) + vad_data->beta * f.p + alpha0;
    }
    break;
  }

  case ST_VOICE: {
    int may_be_voice = (f.p > vad_data->llindar0 || f.zcr > vad_data->llindar_zcr);
    if (may_be_voice) {
      vad_data->hangover = 7; 
    } else {
      vad_data->hangover--;
      if (vad_data->hangover <= 0) {
        vad_data->state = ST_SILENCE;
      }
    }
    break;
  }
  
  case ST_UNDEF:
    /*if (may_be_voice) {
      vad_data->state = ST_VOICE;
      vad_data->hangover = 5;
    } else {
      vad_data->state = ST_SILENCE;
    }*/
    break;
  }

  if (vad_data->state == ST_VOICE)
    return ST_VOICE;
  else
    return ST_SILENCE;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}