//103JTサーミスタのB定数について理解する

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

//各種値保存用
typedef struct {
    int temp_l; //測定温度範囲の上端
    int temp_h; //測定温度範囲の下端
    double r_h; //上端温度のときの抵抗値
    double r_m;
    double r_l; //上端温度のときの抵抗値
    double r_connection; //接続する抵抗
    double b_const; //B定数
} setting;

double assign_resist(int temp);
double derive_r_connection(double r_l, double r_m, double r_h);
double derive_B_const(int temp_l, int temp_h, double r_l, double r_h);
double derive_resistor_from_temp(int temp, double b_const, int temp_l, double r_l);
bool is_valid_temp(int temp);
int decide_subscript_from_temp(int temp);
double derive_middle_resist(int temp_l, int temp_h, double r_l, double r_h, double b_const);
double derive_temp_from_resistance(int temp_l, double b_const, double r_l, double r_t);

//const int STANDARD_TEMP = 25;
const double ABSOLUTE_ZERO = 273.15; //絶対温度への変換


const int setted_temps[] = {
    -50, -40, -30, -20, -10, 0, 10, 20, 25, 30, 40, 50, 60, 70,80, 85, 90, 100, 110, 120, 125
};
const double resistances[] = {
    367.7, 204.7, 118.5, 71.02, 43.67, 27.70, 18.07, 12.11, 10.00, 8.301, 5.811, 4.147, 3.011, 2.224, 1.668, 1.451, 1.267, 0.9753, 0.7597, 0.5981, 0.5331
};
const int set_temp_length = sizeof(setted_temps) / sizeof(setted_temps[0]);

int main(void) {
    setting sett = {
        .temp_l = 0,
        .temp_h = 0,
        .r_l = 0.0,
        .r_m = 0.0,
        .r_h = 0.0,
        .r_connection = 0.0,
        .b_const = 0.0
    };

    printf("input temperature from under place\n");
    //printf("leng %d\n", set_temp_length); //d

    //入力可能温度一覧
    for(int i = 0; i < set_temp_length; i++) {
        printf("%d ", setted_temps[i]);

        //改行
        if ((i + 1) % (set_temp_length / 3) == 0) {
            printf("\n");
        }
    }
    printf("\n");


    /*温度入力*/
    //最低測定温度
    do {
        printf("input low temp: ");
        scanf("%d", &sett.temp_l);
    } while (is_valid_temp(sett.temp_l) != true);
    sett.r_l = assign_resist(sett.temp_l);

    //最高測定温度
    do {
        printf("input high temp: ");
        scanf("%d", &sett.temp_h);
    } while (is_valid_temp(sett.temp_h) != true);
    sett.r_h = assign_resist(sett.temp_h);

    /*B定数導出*/
    sett.b_const = derive_B_const(sett.temp_l,  sett.temp_h, sett.r_l, sett.r_h);

    /*中間温度の抵抗導出 */
    sett.r_m = derive_middle_resist(sett.temp_l, sett.temp_h, sett.r_l, sett.r_h, sett.b_const);

    /*基準抵抗導出*/
    sett.r_connection = derive_r_connection(sett.r_l, sett.r_m, sett.r_h);

    //表示
    printf("\n--- status ---\n");
    printf("temp: \t%d℃ ~ %d℃\n", sett.temp_l, sett.temp_h);
    printf("- resist -\n");
    printf("Rh: %.3f\n",sett.r_h);
    printf("Rm: %.3f\n",sett.r_m);
    printf("Rl: %.3f\n",sett.r_l);

    printf("R_connection: %.3fkΩ\n", sett.r_connection);
    printf("B constant: %.3f\n", sett.b_const);

    double v_cc = 5.0;

    while (1) {
        /*電圧測定*/
        double v_x = 0;
        printf("input voltage: ");
        scanf("%lf", &v_x);

        /*電圧から抵抗値*/
        double r_t = v_x / (v_cc - v_x) * sett.r_connection;
        printf("Rt: %f\n", r_t);

        /*抵抗値から温度*/
        double current_temp = derive_temp_from_resistance(sett.temp_l, sett.b_const, sett.r_l, r_t);
        printf("Current temp: %f\n\n", current_temp);
    }



    return 0;
}


/*ヘルパー関数 */
//温度から配列の添字特定
int decide_subscript_from_temp(int temp) {
    for (int i = 0; i < set_temp_length; i++) {
        if (setted_temps[i] == temp) return i;
    }
    return -1;
}


//温度から添字特定し、抵抗値導出
double assign_resist(int temp) {
    int subscript = decide_subscript_from_temp(temp);

    //抵抗値代入
    return resistances[subscript];
}


//配列内の温度か判定
bool is_valid_temp(int temp) {
    bool flag = false;
    for(int i = 0; i < set_temp_length; i++) {
        if (temp == setted_temps[i]) {
            flag = true;
        }
    }
    return flag;
}


//基準抵抗導出
double derive_r_connection(double r_l, double r_m, double r_h) {
    double molecule = r_m * (r_l + r_h) - 2 * (r_l * r_h);
    double denominator = (r_l + r_h) - 2 * r_m;

    double r_connection = molecule / denominator;

    return r_connection;
}


//B定数導出
double derive_B_const(int temp_l, int temp_h, double r_l, double r_h) {
    double t1 = temp_h + ABSOLUTE_ZERO;
    double t0 = temp_l + ABSOLUTE_ZERO;

    return (log(r_h / r_l)) / ((1.0 / t1) - (1.0 / t0));
}


//R-T特性
double derive_resistor_from_temp(int temp, double b_const, int temp_l, double r_l) {
    double t0 = temp_l + ABSOLUTE_ZERO;
    double r0 = assign_resist(temp_l);

    double t1 = temp + ABSOLUTE_ZERO;

    double index = b_const * ((1.0 / t1) - (1.0 / t0));
    double r = r0 * exp(index);

    printf("T: %d℃  R : %fkΩ\n", temp, r);//d

    return r;
}


//中間の抵抗導出
double derive_middle_resist(int temp_l, int temp_h, double r_l, double r_h, double b_const) {
    int temp_m = (temp_l + temp_h) / 2;

    if (is_valid_temp(temp_m) != true) {
        return derive_resistor_from_temp(temp_m, b_const, temp_l, r_l);
    }
    return assign_resist(temp_m);
}


//抵抗値から温度
double derive_temp_from_resistance(int temp_l, double b_const, double r_l, double r_t) {
    double t0 = temp_l + ABSOLUTE_ZERO;

    double t1 = b_const / (log(r_t / r_l) + b_const * (1.0 / t0));
    return t1 - ABSOLUTE_ZERO;
}


//電圧から抵抗値
double derive_resistance_from_voltage(double v_x, double v_cc, double r_connection) {
    double r_t = v_x / (v_cc - v_x) * r_connection;
    return r_t;
}
