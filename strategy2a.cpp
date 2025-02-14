#ifndef STRATEGY2A_CPP
#define STRATEGY2A_CPP

//
#include "def.h"
#include "strategies.h"
#include "route.h"
//

namespace strategy2a {

const float ratio = 0.55;

QString put_string = "hello world\n\n"; // only for debugging


//
float score(rank_type rank)
{
    switch(rank)
    {
        case 10: return 500;
        case 40: return 40;
        case 39: return 30;
        case 38: return 20;
        case 37: return 10;
        case 36: return 4;
        case 35: return 3;
        case 0: return 23;
        case 30: return 7;
        case 100: return 3;
        default: return 2;
    }
}


//
bool under_attack0(board & b, chess_type chess)
{
    bool result;

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
                        return true;
                    }
                }

                if (beat == 0)
                {
                    pos_list move_list = route_list(b, e_chess, chess.code);
                    bool accessible = (move_list.size() > 1);

                    if (accessible)
                    {
                        float diff = score(e_chess.rank) - score(chess.rank);
                        if (diff <= 0) return true;
                    }

                 }
               }
            }

        }

        return false;

}

//
bool protected_against(board & b, chess_type chess, chess_type e_chess)
{
    bool result = false;

    position p(chess.code);

    if (p.is_camp()) return false;

    pos_list allied_chesses = b.find_allies(right_country(e_chess.belong_to));

    for_int(i, allied_chesses.size())
    {
        int_type m_code = allied_chesses[i];
        position m_pos(m_code);
        chess_type m_chess = b.find_chess(m_code);

        if (m_code == chess.code) continue;

        if (m_chess.movable())
        {
            int beat = beat_it(m_chess.rank, e_chess.rank);

            if (beat == 1)
            {
                pos_list move_list = route_list(b, m_chess, chess.code);
                bool accessible = (move_list.size() > 1);

                if (accessible)
                {
                    return true;
                }
            }

            if (beat == 0)
            {
                pos_list move_list = route_list(b, m_chess, chess.code);
                bool accessible = (move_list.size() > 1);

                if (accessible)
                {
                    float diff = score(m_chess.rank) - score(e_chess.rank);
                    if (diff <= 0) return true;
                }
             }
        }

    }

    return false;

}


//
float under_attack(board & b, chess_type chess)
{
    float result = 0;
    float enemy_under_attack_ratio, protected_against_ratio;

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
                        // test if the enemy chess is under attack
                        if (under_attack0(b, e_chess))
                            enemy_under_attack_ratio = 0.1;
                        else
                            enemy_under_attack_ratio = 1.0;

                        if (protected_against(b, chess, e_chess))
                            protected_against_ratio = 0.15;
                        else
                            protected_against_ratio = 1.0;

                        return -1.0 * enemy_under_attack_ratio * protected_against_ratio;
                    }
                }

                if (beat == 0)
                {
                    pos_list move_list = route_list(b, e_chess, chess.code);
                    bool accessible = (move_list.size() > 1);

                    if (accessible)
                    {
                        float diff = score(e_chess.rank) - score(chess.rank);
                        if (diff >= 0)
                            result = 0;
                        else
                            result = -1.0;

                        // test if the enemy chess is under attack
                        if (under_attack0(b, e_chess))
                            enemy_under_attack_ratio = 0.1;
                        else
                            enemy_under_attack_ratio = 1.0;

                        // no protected against here

                        return result * enemy_under_attack_ratio;
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
                case 0: return 15;
                case 1: return 20;
                case 2: return 15;
                case 3: return 0;
                default: return 0.5;
            }
        else if (row == 4)
            switch (col)
            {
                case 0: return 5;
                case 1: return 12;
                case 2: return 5;
                case 3: return 2;
                default: return 1;
            }
        else if (row == 3)
            switch (col)
            {
                case 1: return 8;
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


float calculate_position_value(board & b, position pos)
{
    int flag_col = 0;
    float result = 0.0;

    if (pos.country == middle) return 0;

    if (b.find_chess(position(pos.country, 5, 1)).is_flag())
        flag_col = 1;
    else if (b.find_chess(position(pos.country, 5, 3)).is_flag())
        flag_col = 3;
    else
        flag_col = 0;

    if (flag_col == 0) return 0;

    float pv = 2.5 * position_value(pos, flag_col);

    //

    chess_type chess = b.find_chess(pos);

    if ((chess.state != empty) && pos.is_camp()) return 0;

    if (chess.state != empty)
        if (is_enemy(chess.belong_to, pos.country))
        {
            if (under_attack0(b, chess)) pv *= 0.15;
            return pv * 1.5;
        }

    pos_list enemy_chesses = b.find_allies(right_country(pos.country));

    for_int(i, enemy_chesses.size())
    {
        int_type e_code = enemy_chesses[i];
        position e_pos(e_code);
        chess_type e_chess = b.find_chess(e_code);

        if (e_chess.movable())
        {
            int beat;
            if (chess.state == empty)
                beat = 1;
            else
                beat = beat_it(e_chess.rank, chess.rank);

            if (beat == 1)
            {
                pos_list move_list = route_list(b, e_chess, chess.code);
                bool accessible = (move_list.size() > 1);

                if (accessible)
                {
                    if (protected_against(b, chess, e_chess))
                        pv *= 0.15;

                    return pv;
                }
           }
        }
    }

    return 0;

}


//
float calculate_board(board & b, country_type now_turn)
{
    float sum = 1000.0, coefficient, max_coefficient;
    col_type flag_col = 0;
    float pv, max_pv = -1000.0;

    loop(i)
    {
        position pos(i);
        pv = calculate_position_value(b, pos);

        if (pv > max_pv)
        {
            max_pv = pv;

            if (is_enemy(pos.country, now_turn))
                max_coefficient = 1;
            else
                max_coefficient = -1;
        }

        chess_type chess = b.find_chess(pos);                
        if (chess.state == empty) continue;

        if (is_enemy(chess.belong_to, now_turn))
            coefficient = -1.0;
        else
            coefficient = 1.0;

        if (is_enemy(chess.belong_to, pos.country))
            coefficient *= 1.05;


        sum += score(chess.rank) * (1 + ratio * under_attack(b, chess))
                * coefficient
                ;

    }

    sum += max_coefficient * max_pv;

    return sum;
}








float calculate_value1(board & b, country_type now_turn)
{
    float sum = 0, coefficient, pos_coefficient;
    col_type flag_col = 0;

    loop(i)
    {
        position pos(i);

        chess_type chess = b.find_chess(pos);
        if (chess.state == empty) continue;

        if (is_enemy(chess.belong_to, now_turn))
        {
            coefficient = -1.0;
        }
        else if (chess.belong_to == now_turn)
        {
            coefficient = 1.0;
        }
        else
        {
            coefficient = 1.0;
        }

        if (is_enemy(chess.belong_to, pos.country))
            coefficient *= 1.05;

        sum += score(chess.rank)
                * coefficient
                ;

    }

    return sum;
}


float calculate_value2(board & b, country_type now_turn)
{
    float sum = 0, coefficient, pos_coefficient;
    col_type flag_col = 0;

    loop(i)
    {
        position pos(i);

        chess_type chess = b.find_chess(pos);
        if (chess.state == empty) continue;

        if (is_enemy(chess.belong_to, now_turn))
        {
            coefficient = -1.0;
        }
        else if (chess.belong_to == now_turn)
        {
            coefficient = 1.0;
        }
        else
        {
            coefficient = 1.0;
        }

        if (is_enemy(chess.belong_to, pos.country))
            coefficient *= 1.05;

        sum += score(chess.rank) * ratio * under_attack(b, chess)
                * coefficient
                ;

    }

    return sum;
}



float calculate_value3(board & b, country_type now_turn)
{
    float sum = 0, max_coefficient;
    col_type flag_col = 0;
    float pv, max_pv = -1000.0;

    loop(i)
    {
        position pos(i);
        pv = calculate_position_value(b, pos);

        if (pv > max_pv)
        {
            max_pv = pv;

            if (is_enemy(pos.country, now_turn))
                max_coefficient = 1;
            else
                max_coefficient = -1;
        }

    }

    sum += max_coefficient * max_pv;

    return sum;
}















//
//
move_type run_strategy(board & b, country_type belong_to)
{
    pos_list whole_list = b.find_belong_to(belong_to);
    move_type one_move;
    float value, max_value = -10000;

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

                        value = calculate_board(b2, belong_to);

                        //
                        if (b2.debug2)
                        {
                            float value1 = calculate_value1(b2, belong_to);
                            float value2 = calculate_value2(b2, belong_to);
                            float value3 = calculate_value3(b2, belong_to);

                            put_string += country_name(s_pos.country) + " ";
                            put_string += rank_code(s_chess.rank) + " ";
                            put_string += "走至  ";
                            put_string += country_name(d_pos.country) + "(";
                            put_string += QString().number(d_pos.row + 1) + ", " +
                                          QString().number(d_pos.col + 1) + ") ";
                            put_string += ", 价值 ";
                            put_string += QString("%1").arg(value);

                            put_string += " = 1000 + "
                                                + QString("%1").arg(value1) + " + "
                                                + QString("%1").arg(value2) + " + "
                                                + QString("%1").arg(value3);

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

} // namespace strategy2a
//

#endif // STRATEGY2A_CPP
