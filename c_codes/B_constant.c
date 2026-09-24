//103JTサーミスタのB定数について理解する

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

//各種設定保存用
typedef struct {
    int temp_l;
    int temp_h;
    double resist_l;
    double resist_h;
    double r_standard;
    double b_const;
} setting;

double assign_resist(int temp);
double derive_r_resist(double r_l, double r_h);
double derive_B_const(int temp_l, int temp_h, double r_l, double r_h);

bool is_valid_temp(int temp);
int decide_subscript_from_temp(int temp);
double derive_middle_resist(int temp1, int temp2, double r_l, double r_h);


const int setted_temps[] = {
    -50, -40, -30, -20, -10, 0, 10, 20, 25, 30, 40, 50, 60, 70,80, 85, 90, 100, 110, 120, 125
};
const double resistances[] = {
    367.7, 204.7, 118.5, 71.02, 43.67, 27.70, 18.07, 12.11, 10.00, 8.301, 5.811, 4.147, 3.011, 2.224, 1.448, 1.451, 1.267, 0.9753, 0.7597, 0.5981, 0.5331
};
const int set_temp_length = sizeof(setted_temps) / sizeof(setted_temps[0]);

int main(void) {
    setting sett = {
        .temp_l = 0,
        .temp_h = 0,
        .resist_l = 0.0,
        .resist_h = 0.0,
        .r_standard = 0.0,
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
    sett.resist_l = assign_resist(sett.temp_l);

    //最高測定温度
    do {
        printf("input high temp: ");
        scanf("%d", &sett.temp_h);
    } while (is_valid_temp(sett.temp_h) != true);
    sett.resist_h = assign_resist(sett.temp_h);


    /*基準抵抗導出*/
    sett.r_standard = 0;

    /*B定数導出*/
    sett.b_const = derive_B_const(sett.temp_l,  sett.temp_h, sett.resist_l, sett.resist_h);



    //表示
    printf("\n--- status ---\n");
    printf("temp: \t%d℃ ~ %d℃\n", sett.temp_l, sett.temp_h);
    printf("resist:\t%f kΩ ~ %f kΩ\n", sett.resist_h, sett.resist_l);
    printf("standard resist: %f\n", sett.r_standard);
    printf("B constant: %f\n", sett.b_const);

}


//基準抵抗導出
double derive_r_resist(double r_l, double r_h) {

    double r_m = 0;
}


//B定数導出
double derive_B_const(int temp_l, int temp_h, double r_l, double r_h) {
    return (log(r_h / r_l)) / ((1.0 / (temp_h + 273.15)) - (1.0 / (temp_l + 273.15)));
}


//正しい温度か判定
bool is_valid_temp(int temp) {
    bool flag = false;
    for(int i = 0; i < set_temp_length; i++) {
        if (temp == setted_temps[i]) {
            flag = true;
        }
    }
    return flag;
}


//温度から添字特定し、抵抗値導出
double assign_resist(int temp) {
    int subscript = decide_subscript_from_temp(temp);

    //抵抗値代入
    return resistances[subscript];
}


//温度から配列の添字特定
int decide_subscript_from_temp(int temp) {
    for (int i = 0; i < set_temp_length; i++) {
        if (setted_temps[i] == temp) return i;
    }
}


//中間の抵抗導出
double derive_middle_resist(int temp_l, int temp_h, double r_l, double r_h) {
    int temp_m = (temp_l + temp_h) / 2;

    if (is_valid_temp(temp_m) != true) {

    }
    return assign_resist(temp_m);
}
