#include "main.h"
#include "parameter.h"
#include "object.h"
#include "def.h"
#include "drawboard.h"
#include "drawchess.h"
#include "utils.h"
#include "route.h"
#include "strategies.h"

#include <QApplication>
#include <QPainter>
#include <QRectF>
#include <QMouseEvent>
#include <QMessageBox>

#include <QTime>

QString player_name(player_type p)
{
    switch(p)
    {
        case human: return "人类";
        case st0:   return "ST0";
        case st1:   return "ST1";
        case st2:   return "ST2";
        case st3:   return "ST3";

        case st1a:  return "ST1A";
        case st2a:  return "ST2A";
        case st4:   return "ST4";

        default:    return "未知";
    }
}

player_type player(country_type country)
{
    switch(country)
    {
        case down:  return human; break;
        case up:
        case right:
        case left:  return st2a;
        default: throw("problem encountered in player()");
    }
}

void MyMainWindow::paintEvent(QPaintEvent *)
{

    paint->begin(this);
    redraw();
    paint->end();
}

void MyMainWindow::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (b.debug2 && pause)
        {
            pause = false;
            move_to(this_move.from, this_move.to);
            go_to_next_country();
        }
        else
        {
            int_type position_code = search_xy(event->x(), event->y());
            if (position_code != NOPOSITION)
            {
                click_pos(position_code);
            }
        }
    }
}

bool board::forbidden(rank_type rank, country_type country, row_type row, col_type col)
{
    position p(country, row, col);

    if ((rank == 10) && !p.is_base())
        return true;

    if (p.is_base() && !((rank == 10) || (rank == 100) || (rank == 33)))
        return true;

    if ((rank == 100) && (row <= 3))
        return true;

    if ((rank == 0) && (row == 0))
        return true;

    if ((rank == 100) && (row == 4) && (col % 2 == 0))
    {
        rank_type its_rank = find_chess(position(country, 5, col)).rank;
        if ((its_rank == 40) || (its_rank == 39) || (its_rank == 38) || (its_rank == 0))
            return true;
    }

    if ((row == 5) && (col % 2 == 0) &&
        ((rank == 40) || (rank == 39) || (rank == 38) || (rank == 0)))
    {
        rank_type its_rank = find_chess(position(country, 4, col)).rank;
        if (its_rank == 100)
            return true;
    }

    return false;
}

void board::init_board()
{
    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);

    country_type country;
    rank_type rank;
    row_type row;
    col_type col;

    if (debug)
    {
        occupy(position(down, 5, 1), 10, down, normal);
        occupy(position(up, 5, 1), 10, up, normal);
        occupy(position(left, 5, 1), 10, left, normal);
        occupy(position(right, 5, 3), 10, right, normal);

        occupy(position(right, 5, 4), 100, right, normal);

        occupy(position(right, 5, 2), 33, right, normal);

        occupy(position(right, 4, 4), 39, right, normal);
        occupy(position(right, 4, 3), 100, right, normal);
        occupy(position(right, 3, 3), 0, right, normal);

        occupy(position(up, 5, 0), 100, up, normal);
        occupy(position(up, 4, 1), 100, up, normal);
        occupy(position(up, 3, 1), 0, up, normal);
        occupy(position(up, 4, 0), 38, up, normal);


        occupy(position(down, 1, 1), 33, up, normal);

        occupy(position(down, 5, 0), 100, down, normal);
        occupy(position(down, 5, 2), 100, down, normal);
        occupy(position(down, 4, 1), 100, down, normal);
        occupy(position(down, 3, 1), 35, down, normal);

        occupy(position(left, 5, 0), 38, left, normal);
        occupy(position(left, 4, 0), 100, left, normal);
        occupy(position(left, 4, 2), 33, left, normal);

    }

    else
    {
    for_country(belong_to, counter)
    {
        for_int(i, whole_rank_size)
        {
            country = belong_to;
            rank = whole_rank_set[i];

            do
            {
                row = qrand() % row_num(belong_to);
                col = qrand() % col_num(belong_to);

            } while (is_occupied(position(country, row, col)) ||
                     position(country, row, col).is_camp() ||
                     forbidden(rank, country, row, col));

            occupy(position(country, row, col), rank, belong_to, normal);

        }
    }
    }
}

MyMainWindow::MyMainWindow(QWidget *parent):QWidget(parent)
{
    b.debug = false;  // may change
    b.debug2 = false; // user can alter here

    if (!b.debug2)
        setGeometry(40, 40, frame_size, frame_size);
    else
        setGeometry(40, 40, frame_size * 1.7, frame_size * 1.3);

    setWindowTitle("米勒酷四国军棋");

    paint = new QPainter;

    b.init_board();
    pause = false;

    if (b.debug2)
    {
        label = new QLabel(this);
        QFont font;
        font.setPointSize(10);

        label->setGeometry(QRect(QPoint(frame_size*1.0, frame_size*0.07),
                                  QSize(frame_size*1.2, frame_size*1.8)
                                 ));

        label->setFont(font);
        label->setAlignment(Qt::AlignLeft);

    }

}

MyMainWindow::~MyMainWindow()
{
    delete paint;
}

//
void MyMainWindow::redraw()
{
    draw_board(paint);
    draw_all_chesses(b, paint);

    for_country(country, counter)
    {
        if (b.which_turn == country)
            draw_extra(paint, country, "该走", extra);
        else
            draw_extra(paint, country, player_name(player(country)), normal);
    }


}

//
move_type computer_run(board & b, country_type country, player_type pl)
{    
    switch(pl)
    {
        case st1: return strategy1::run_strategy(b, country);
        case st2: return strategy2::run_strategy(b, country);
        case st3: return strategy3::run_strategy(b, country);

        case st1a: return strategy1a::run_strategy(b, country);
        case st2a: return strategy2a::run_strategy(b, country);
        //case st4:  return strategy4::run_strategy(b, country);

        default: throw("Wrong strategy name!");
    }
}

//
void MyMainWindow::go_to_next_country()
{
    b.which_turn ++;

    if (b.cannot_move(b.which_turn))
        b.delete_belong_to(b.which_turn); // important

    if (b.is_empty(b.which_turn))
    {
        if (b.is_empty(ally_country(b.which_turn)))
        {
            return;
        }
        go_to_next_country();
    }
    else if (player(b.which_turn) != human)
    {
        repaint();
        move_type move = computer_run(b, b.which_turn, player(b.which_turn));

        if (b.debug2)
        {
            label->setText(move.text);
            label->show();

            repaint();

            this_move = move;
            pause = true;

        }
        else
        {
            move_to(move.from, move.to);
            go_to_next_country();
        }
    }
}

//
void MyMainWindow::draw_route(pos_list move_list,
                              rank_type rank, country_type belong_to,
                              float time = 0.1)
{
    size_t len = move_list.size();
    for(size_t i = 0; i < len; i ++)
    {
        int_type pos = move_list[i];
        b.remove_position(pos);
        b.occupy(pos, rank, belong_to, picked_up);
        repaint();

        // sleep for some time
        QTime t;
        t.start();
        while (t.elapsed() < 1000*time/len);

        b.remove_position(pos);
    }
}


//
void MyMainWindow::move_to(int_type from_code, int_type to_code)
{
    position from(from_code);
    position to(to_code);

    chess_type from_chess = b.find_chess(from);
    chess_type to_chess = b.find_chess(to);

    pos_list move_list = route_list(b, from_chess, to_code);
    bool accessible = (move_list.size() > 1);

    if (accessible)
        if (b.go_able(from_chess, to))
        {
            draw_route(move_list, from_chess.rank, from_chess.belong_to, 1.0);

            b.go_to(from_chess, to_chess);
        }

}

//
void MyMainWindow::click_pos(int_type position_code)
{
    static bool just_start = true;

    if (position_code == NOPOSITION) return;
    if (!just_start && (player(b.which_turn) != human)) return;

    position p(position_code);
    chess_type c = b.find_chess(p);

    int_type picked_pos;

    pos_list picked_list = b.find_state(picked_up);
    size_t len = picked_list.size();

    if (len > 1)
        throw("picked up more than one chess in click_pos()");
    else if (len == 1)
        picked_pos = picked_list[0];
    else
        picked_pos = NOPOSITION;

    if ((picked_pos == NOPOSITION) && (c.state != empty))
        if (b.which_turn == c.belong_to)
          if ((c.movable()) && (!p.is_base()))

            b.change_state(p, picked_up);

    if (picked_pos != NOPOSITION) // nothing has been picked up
    {
       chess_type picked_chess = b.find_chess(picked_pos);

       pos_list move_list = route_list(b, picked_chess, position_code);
       bool accessible = (move_list.size() > 1);

       if (!accessible)
       {
            b.change_state(picked_pos, normal);
       }

       else if (b.go_able(picked_chess, p))
       {

           draw_route(move_list, picked_chess.rank, picked_chess.belong_to);

           b.go_to(picked_chess, c);

           just_start = false;

           go_to_next_country();

       }

    }

    repaint();
}

//
void MyMainWindow::celebrate(country_type country)
{

    paint->setFont(QFont("Times", lsize*4));

    switch(country)
    {
        case left:
        case right:
            paint->drawText(QRectF(QPointF(150, 150), QSizeF(300, 300)), Qt::AlignCenter, "东西胜利");
        break;
        case up:
        case down:
            paint->drawText(QRectF(QPointF(150, 150), QSizeF(300, 300)), Qt::AlignCenter, "南北胜利");
        break;
        default:;

    }

}


//
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MyMainWindow w;

    w.show();

    return a.exec();
}
