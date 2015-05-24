#pragma once

float math_sqrt(const float num)
{
    const int MAX_STEPS = 40;
    const float MAX_ERROR = 0.001;
  
    float answer = num;
    float ans_sqr = answer * answer;
    int step = 0;
    while ((ans_sqr - num > MAX_ERROR) && (step++ < MAX_STEPS))
    {
        answer = (answer + (num / answer)) / 2;
        ans_sqr = answer * answer;
    }
    return answer;
}

float math_dist2d(int x1, int y1, int x2, int y2)
{
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    return math_sqrt((dx * dx) + (dy * dy));
}

int math_min(int lhs, int rhs)
{
    return (lhs <= rhs) ? lhs : rhs;
}