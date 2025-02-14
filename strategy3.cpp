#ifndef STRATEGY3_CPP
#define STRATEGY3_CPP

//
#include "def.h"
#include "strategies.h"
#include "route.h"
//



namespace strategy3 {

const float ratio = 0.25; // can not exceed 1.0






//
float score(rank_type rank)
{
    switch(rank)
    {
        case 10: return 500;
        case 40: return 40;
        case 39: return 33;
        case 0:  return 19;
        case 38: return 18;
        case 37: return 10;
        case 36: return 7;
        case 35: return 4;
        case 34: return 3;
        case 33: return 1;
        case 30: return 5;
        case 100: return 3;
        default: return 2;
    }
}

//
float position_score(rank_type rank)
{
    switch(rank)
    {
        case 40: return 30;
        case 39: return 25;
        case 38: return 20;
        case 37: return 15;
        case 36: return 10;
        case 35: return 8;
        case 0: return 22;
        default: return 5;
    }
}

//
float under_attack_ratio(board & b, chess_type chess)
{
    float result = 1.0;

    position p(chess.code);

    if ((chess.state != empty) && !p.is_camp())
    {
        pos_list enemy_chesses = b.find_allies(right_country(chess.belong_to));

        for_int(i, enemy_chesses.size())
        {
            int_type e_code = enemy_chesses[i];
            position e_pos(e_code);
            chess_type e_chess = b.find_chess(e_code);

            if (e_chess.movable())
            {
                int beat = beat_it(e_chess.rank, chess.rank);

                if (beat == 1)
                {
                    pos_list move_list = route_list(b, e_chess, chess.code);
                    bool accessible = (move_list.size() > 1);

                    if (accessible)
                    {
                        return 1.0 - ratio;
                    }
                }

                if (beat == 0)
                {
                    pos_list move_list = route_list(b, e_chess, chess.code);
                    bool accessible = (move_list.size() > 1);

                    if (accessible)
                    {
                        result = 1.0 - (1.0 - score(e_chess.rank)/score(chess.rank)) * ratio;
                        if (result > 1.0) result = 1.0;
                        return result;
                    }

                 }
               }
            }

        }

        return result;

}

//
float position_value(position pos, col_type flag_col)
{
    if (pos.country == middle) return 1;

    row_type row = pos.row;
    col_type col = pos.col;

    if (flag_col == 3)

        return position_value(position(pos.country, row, 4 - col), 1);

    else // flag_col == 1

        if (row == 5)
            switch (col)
            {
                case 0: return 5;
                case 1: return 0;
                case 2: return 5;
                case 3: return 0;
                default: return 1;
            }
        else if (row == 4)
            switch (col)
            {
                case 0: return 5;
                case 1: return 6;
                case 2: return 5;
                case 3: return 2;
                default: return 1;
            }
        else if (row == 3)
            switch (col)
            {
                case 1: return 5;
                case 2: return 2;
                case 3: return 2;
                default: return 1;
            }
        else if (row == 2)
            switch (col)
            {
                case 2: return 4;
                default: return 1;
            }
        else
            return 1;

}



//
 float calculate_value(board & b, country_type belong_to)
 {
     pos_list all_chess = b.find_belong_to(belong_to);
     float sum = 0;

     if (b.is_empty(belong_to)) return 0;

     for_int(i, all_chess.size())
     {
         int_type pos = all_chess[i];

         chess_type chess = b.find_chess(pos);

         sum += score(chess.rank) * under_attack_ratio(b, chess);

     }

     return sum;
 }

 //
 float calculate_position_value(board & b, country_type belong_to)
 {
     pos_list all_chess = b.find_belong_to(belong_to);
     float sum = 0;
     col_type flag_col;

     if (b.is_empty(belong_to)) return 0;

     if (b.find_chess(position(belong_to, 5, 1)).is_flag())

         flag_col = 1;

     else if (b.find_chess(position(belong_to, 5, 3)).is_flag())

         flag_col = 3;

     else

         flag_col = 0;

     pos_list my_all_chess = b.find_country(belong_to);

     for_int(i, my_all_chess.size())
     {
         int_type my_pos = my_all_chess[i];

         chess_type my_chess = b.find_chess(my_pos);

         if (my_chess.state != empty)
         {
             float sign = (is_enemy(my_chess.belong_to, belong_to))?-1:2/5;

             float under_attack_r;

             if (under_attack_ratio(b, my_chess) > 0.99)
                 under_attack_r = 1.0;
             else
                 under_attack_r = 0.15;


             sum += sign/10 * position_score(my_chess.rank) *
                              position_value(position(my_pos), flag_col) *
                              under_attack_r;
         }

     }

     return sum;

 }











//
//
move_type run_strategy(board & b, country_type belong_to)
{
    pos_list whole_list = b.find_belong_to(belong_to);
    move_type one_move;
    float value, new_value, max_value = -10000.0, min_new_value;

    QString put_string = "hello world\n\n"; // only for debugging

    int run_num = 0;

    for_int(i, whole_list.size())
    {
        int_type s_code = whole_list[i];
        position s_pos(s_code);
        chess_type s_chess = b.find_chess(s_code);

        if (s_chess.movable() && !s_pos.is_base())

            loop(d_code)
            {
                position d_pos(d_code);

                chess_type d_chess = b.find_chess(d_pos);

                bool eat_shit = d_pos.is_base() && (!d_chess.is_flag());

                pos_list move_list = route_list(b, s_chess, d_code);
                bool accessible = (move_list.size() > 1);

                if (accessible)
                {
                    if (b.go_able(s_chess, d_pos) && (!eat_shit))
                    {

                        board b2 = b;

                        b2.go_to(s_chess, d_chess);

                        /////////////////////////////

                        min_new_value = 10000.0;

                        country_type belong_to2 = right_country(belong_to);

                        while (b2.is_empty(belong_to2))
                            belong_to2 = right_country(belong_to2);

                        if (belong_to2 == belong_to) break;


                        pos_list whole_list2 = b2.find_belong_to(belong_to2);

                        for_int(i2, whole_list2.size())
                        {

                            int_type s_code2 = whole_list2[i2];
                            position s_pos2(s_code2);
                            chess_type s_chess2 = b2.find_chess(s_code2);

                            if (s_chess2.movable() && !s_pos2.is_base())
                            loop(d_code2)
                            {
                                position d_pos2(d_code2);
                                chess_type d_chess2 = b2.find_chess(d_pos2);

                                bool eat_shit2 = d_pos2.is_base() && (!d_chess2.is_flag());

                                pos_list move_list2 = route_list(b2, s_chess2, d_code2);
                                bool accessible2 = (move_list2.size() > 1);

                                if (accessible2)
                                if (b2.go_able(s_chess2, d_pos2) && (!eat_shit2))
                                {

//                                    run_num ++;

                                    board b3 = b2;

                                    b3.go_to(s_chess2, d_chess2);

                                    float value1 = calculate_value(b3, belong_to),
                                          value2 = calculate_position_value(b3, belong_to),
                                          value3 = calculate_value(b3, ally_country(belong_to)),
                                          value4 = calculate_value(b3, right_country(belong_to)),
                                          value5 = calculate_value(b3, left_country(belong_to)),
                                          value6 = calculate_position_value(b3, right_country(belong_to)),
                                          value7 = calculate_position_value(b3, left_country(belong_to))
                                            ;

                                    new_value = value1 + value2 + value3 - value4 - value5 - value6 - value7;

                                    //

                                    if (new_value <= min_new_value)
                                    {
                                       min_new_value = new_value;
                                    }
                                }
                            }
                        }

                        value = min_new_value;

                        if (b.debug2)
                        {
                                put_string += country_name(s_pos.country) + " ";
                                put_string += rank_code(s_chess.rank) + " ";
                                put_string += "走至  ";
                                put_string += country_name(d_pos.country) + "(";
                                put_string += QString().number(d_pos.row + 1) + ", " +
                                              QString().number(d_pos.col + 1) + ") ";
                                put_string += ", 价值 ";
                                put_string += QString("%1").arg(value);

                                put_string += "\n";
                       }

                       //

                       value += qrand() % 5;

                       if (value >= max_value)
                       {
                           max_value = value;

                           one_move.from = s_code;
                           one_move.to = d_code;
                       }

                    }

            }
       }
  }


  if (b.debug2)
      one_move.text = put_string;
  else
      one_move.text = "";

  return one_move;


}


} // namespace strategy3
//

#endif // STRATEGY3_CPP
