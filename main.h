#ifndef MAIN_H
#define MAIN_H

#include "object.h"
#include "def.h"

//
#include <QWidget>
#include <QLabel>

//

//
typedef enum {
    human,
    st0,
    st1,
    st2,
    st3,

    st1a,
    st2a,
    st4

} player_type;


//
class MyMainWindow: public QWidget
{

private:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

    bool pause; // only for debugging
    move_type this_move; // only for debugging

//    QImage * image;
    QPainter * paint;

    board b;

    QLabel * label; // only for debugging



public:
    MyMainWindow(QWidget * parent = 0);
    ~MyMainWindow();

    void redraw();
    void click_pos(int_type);
    void draw_route(pos_list, rank_type, country_type, float);

    void go_to_next_country();
    void move_to(int_type, int_type);

    void celebrate(country_type);

};

//


#endif // MAIN_H
