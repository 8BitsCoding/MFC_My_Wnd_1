#pragma once

#define		X_COUNT		45
#define		Y_COUNT		100

#define		WIDTH		10
#define		HEIGHT		10

struct ProgressData
{
	int data;
	int status;			// 0 미처리, 1 정상, 2 오류
};
// ProgressStatusWnd

class ProgressStatusWnd : public CWnd
{
	DECLARE_DYNAMIC(ProgressStatusWnd)

public:
	ProgressStatusWnd();
	virtual ~ProgressStatusWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	int SetData(int a_data, int a_cur_pos_check);
	void RedrawData();
	void RedrawAllData();
	void MoveCurrentPos();
	void ResetData();

	CImage m_draw_image;
	CDC m_draw_dc;

	CBrush m_normal_brush, m_error_brush, m_no_data_brush;
	CPen m_normal_pen, m_error_pen, m_no_data_pen;

	int m_display_y_count, m_count = 0;
	//ProgressData **mp_data;
	ProgressData *mp_data;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void ProcessScroll(UINT nSBCode, UINT nPos, int a_scroll_type);
};


