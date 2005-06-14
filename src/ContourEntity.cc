/**
 * $Header$
 */

#include <total/ContourEntity.hh>

namespace Total {

  void ContourEntity::SetStart(int x, int y) {
    m_points.clear();
    m_currentX = m_centralX = m_minX = m_maxX = x;
    m_currentY = m_centralX = m_minY = m_maxY = y;
    m_numPoints = 1;
    m_length=32;
    m_points.push_back(x);
    m_points.push_back(y);
  }

  void ContourEntity::AddPoint(int freeman) {
    assert(freeman >= -1 && freeman < 8);

    if (freeman == -1) return;

    static int offset_x[] = {-1,-1,0,1,1,1,0,-1};
    static int offset_y[] = {0,1,1,1,0,-1,-1,-1};
    bool is4connected = freeman == 0 || freeman == 6 || freeman == 4 || freeman == 2;
    int newX = m_currentX += offset_x[freeman];
    int newY = m_currentY += offset_y[freeman];
    
    m_length += is4connected ? 32 : 45;
    if (newX < m_minX) m_minX = newX;
    if (newX > m_maxX) m_maxX = newX;
    if (newY < m_minY) m_minY = newY;
    if (newY > m_maxY) m_maxY = newY;
    m_centralX += newX;
    m_centralY += newY;
    m_currentX = newX;
    m_currentY = newY;
    ++m_numPoints;
    m_points.push_back(newX);
    m_points.push_back(newY);
  }

}
