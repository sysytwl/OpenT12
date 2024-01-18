#ifndef beep_h
#define beep_h

#include <Arduino.h>

enum CALCULATORMUSICTONE {
    CMT_NULL = 255,
    CMT_1 = 0,
    CMT_2 = 2,
    CMT_3 = 3,
    CMT_4 = 5,
    CMT_5 = 7,
    CMT_6 = 9,
    CMT_7 = 11,
    CMT_8 = 12,
    CMT_9 = 14,
    CMT_P = 15,
    CMT_S = 17,
    CMT_M = 18,
    CMT_D = 21,
    CMT_E = 22,
};

struct TONE{
    note_t note;
    uint8_t rp;
    uint16_t delay;
};

TONE testSound[] {
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_5,250},
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_8,250},
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_NULL,250},

    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_5,250},
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_4,250},
    {NOTE_D,CMT_5,250},
    {NOTE_D,CMT_NULL,250},
    
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_5,250},
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_M,250},
    {NOTE_D,CMT_NULL,250},

    {NOTE_MAX,255,0},
};

TONE BootSound[]{
    {NOTE_D,CMT_5,230},
    {NOTE_D,CMT_7,230},
    {NOTE_D,CMT_9,215},
    {NOTE_D,CMT_M,215},
    {NOTE_MAX,0,0},
};

TONE RxConnected[]{
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_M,250},
    {NOTE_MAX,0,0},
};

TONE RxLostConnection[]{
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_5,250},
    {NOTE_MAX,0,0},
};

TONE Beep1[]{
    {NOTE_D,CMT_8,50},
    {NOTE_MAX,0,0},
};

TONE Beep2[]{
    {NOTE_D,CMT_M,50},
    {NOTE_D,CMT_NULL,50},
    {NOTE_D,CMT_M,50},
    {NOTE_MAX,0,0},
};

TONE Beep3[]{
    {NOTE_D,CMT_7,50},
    {NOTE_D,CMT_9,50},
    {NOTE_D,CMT_M,50},
    {NOTE_MAX,0,0},
};

class beeper {
public:
    beeper(uint8_t BEEP_PIN, uint8_t BEEP_Channel, uint8_t volume = true) {
        pinMode(BEEP_PIN, OUTPUT);
        ledcAttachPin(BEEP_PIN, BEEP_Channel);
        
        _BEEP_Channel = BEEP_Channel;
        _BEEP_PIN = BEEP_PIN;
    }

    void volume(uint8_t volume){
        _volume = volume;
    }

    void SetSound(TONE sound[]) {
        MySound = sound;
        PlayTones_Schedule = 0;
    }

    void ICACHE_RAM_ATTR SoundUpdate(void) {
        if (!_volume) {
            MySound = NULL;
            SetTone(0);
        }
        if (MySound==NULL) return;
        PlayTones(MySound, &PlayTones_Schedule);
    }

private:
    uint8_t _BEEP_PIN;
    uint8_t _BEEP_Channel;
    uint8_t _volume = true;

    uint16_t PlayTonesDelayTime = 0;
    uint16_t PlayTones_Schedule = 0;
    uint32_t PlayTonesTimer = 0;
    TONE     *MySound = NULL;

    /*** 
     * @description: 发出指定音符的音调
     * @param {note_t} note 绝对音高
     * @param {uint8_t} rp 相对音高（全音）
     * @return {double} noteFreq
     * https://www.zhihu.com/question/27166538/answer/35507740
     * 王赟 Maigo ：
     * 音名是用字母CDEFGAB表示的，它表示的是绝对音高。一个八度中有12个音，分别是C, #C, D, #D, E, F, #F, G, #G, A, #A, B，相邻两个音之间的距离叫一个半音，两个半音叫一个全音。在最常见的定律法——十二平均律中，中央C（钢琴最中间的C音）右边的第一个A音被定义为440 Hz，然后其它音的频率用等比数列推出，相隔半音的两个音的频率之比为pow(2,1/12.0)。由此可以推断出中央C的频率为261 Hz，它右边的#C的频率为277 Hz，D的频率为293 Hz，等等等等。
    唱名指的是do, re, mi这些，可以用数字1234567表示。一个八度中也有12个音，分别是1, #1, 2, #2, 3, 4, #4, 5, #5, 6, #6, 7，相邻两个音之间的距离也是半音。一首曲子是什么调，就是说哪个音名对应着唱名1。所以，唱名表示的是相对音高。比如C调就是说C音是1，这样1的频率就是261 Hz，#1的频率就是277 Hz，2的频率就是293 Hz，等等。而在#C调中，就变成了1是277 Hz，#1是293 Hz，等等。调式总共有12种，你可以按上述方法自己推算。
     */
    double GetNote(note_t note, uint8_t rp) {
        const uint16_t noteFrequencyBase[12] = {
            //   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
                4186,    4435,    4699,    4978,    5274,    5588,    5920,    6272,    6645,    7040,    7459,    7902
        };
        const uint8_t octave          = 4;
        const double  FreqRatio       = 1.059463094; // pow(2,1/12.0);
        // const double  FreqRatioDouble = 1.122462048; // pow(2,1/6.0);
        double noteFreq = ((double)noteFrequencyBase[note] / (double)(1 << (8 - octave))) * pow(FreqRatio,rp);
        //SetTone(noteFreq);
        return noteFreq;
    }

    void PlayTones(TONE* sound, uint16_t* Schedule) {
        if (millis() - PlayTonesTimer > PlayTonesDelayTime) {
            if (sound == NULL || Schedule == NULL || sound[*Schedule].note == NOTE_MAX || !_volume) {
                if (_volume && sound[*Schedule].rp == 255) {
                    *Schedule = 0; //循环播放
                }else{
                    SetTone(0);
                    return;
                }
            }
            //下一个音符
            if (sound[*Schedule].rp == CMT_NULL) SetTone(0);
            else SetTone(GetNote(sound[*Schedule].note, sound[*Schedule].rp));
            PlayTonesTimer = millis();
            //设置延时时间
            PlayTonesDelayTime = sound[*Schedule].delay;
            (*Schedule)++;
        }
        return;
    }

    void SetTone(double freq) {
        ledcWriteTone(_BEEP_Channel, freq);
    }

    // void SetNote(note_t note, uint8_t rp) {
    //     SetTone(GetNote(note, rp));
    // }

};
#endif
