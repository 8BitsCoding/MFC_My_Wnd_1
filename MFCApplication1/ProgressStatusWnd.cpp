// ProgressStatusWnd.cpp: 구현 파일
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "ProgressStatusWnd.h"


// ProgressStatusWnd

IMPLEMENT_DYNAMIC(ProgressStatusWnd, CWnd)

ProgressStatusWnd::ProgressStatusWnd()
{

}

ProgressStatusWnd::~ProgressStatusWnd()
{
}


BEGIN_MESSAGE_MAP(ProgressStatusWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()



// ProgressStatusWnd 메시지 처리기




void ProgressStatusWnd::OnPaint()
{
	CPaintDC dc(this);

	m_draw_image.Draw(dc, 0, 0);
}


int ProgressStatusWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 2차원 메모리 할당
	// mp_data = new ProgressData *[Y_COUNT];
	// for (int i = 0; i < Y_COUNT; i++) *(mp_data + i) = new ProgressData[X_COUNT];
	mp_data = new ProgressData [Y_COUNT*X_COUNT];
	memset(mp_data, 0, sizeof(ProgressData)*Y_COUNT*X_COUNT);

	CRect r;
	GetClientRect(r);

	m_draw_image.Create(r.Width(), r.Height(), 32);
	m_draw_dc.Attach(m_draw_image.GetDC());

	m_normal_brush.CreateSolidBrush(RGB(0, 255, 128));
	m_error_brush.CreateSolidBrush(RGB(255, 0, 128));
	m_no_data_brush.CreateSolidBrush(RGB(64, 64, 64));

	m_normal_pen.CreatePen(PS_SOLID, 1, RGB(0, 200, 100));
	m_error_pen.CreatePen(PS_SOLID, 1, RGB(200, 0, 100));
	m_no_data_pen.CreatePen(PS_SOLID, 1, RGB(192, 192, 192));

	m_draw_dc.SelectObject(&m_no_data_brush);
	m_draw_dc.SelectObject(&m_no_data_pen);

	m_display_y_count = r.Height() / HEIGHT;
	if (r.Height() % HEIGHT) m_display_y_count++;

	SetScrollRange(SB_VERT, 0, Y_COUNT - m_display_y_count, 0);

	for (int y = 0; y < m_display_y_count; y++) {
		for (int x = 0; x < X_COUNT; x++) {
			m_draw_dc.Rectangle(x * WIDTH, y * HEIGHT, (x + 1)*WIDTH, (y + 1)*HEIGHT);
		}
	}

	return 0;
}


void ProgressStatusWnd::OnDestroy()
{
	CWnd::OnDestroy();

	m_draw_dc.DeleteDC();
	m_draw_image.ReleaseDC();
	m_draw_image.Destroy();

	m_normal_brush.DeleteObject();
	m_error_brush.DeleteObject();
	m_no_data_brush.DeleteObject();

	m_normal_pen.DeleteObject();
	m_error_pen.DeleteObject();
	m_no_data_pen.DeleteObject();

	// for (int i = 0; i < Y_COUNT; i++) delete[] * (mp_data + i);
	// delete[] mp_data;
	delete[] mp_data;
}

void ProgressStatusWnd::MoveCurrentPos()
{
	int scroll_pos = GetScrollPos(SB_VERT);
	int y = (m_count + 1) / X_COUNT;

	if (y < scroll_pos || y >= scroll_pos + m_display_y_count - 1) {
		int new_pos = y - 2;
		int max, min;
		GetScrollRange(SB_VERT, &min, &max);
		if (new_pos < min) new_pos = min;
		else if (new_pos > max) new_pos = max;

		if (scroll_pos != new_pos) {
			SetScrollPos(SB_VERT, new_pos);
			RedrawAllData();
		}
	}
}

int ProgressStatusWnd::SetData(int a_data, int a_cur_pos_check)
{
	if (m_count < X_COUNT * Y_COUNT) {
		(mp_data + m_count)->data = a_data;
		if (a_data > 3 && a_data << 97) {
			// Error 없음
			(mp_data + m_count)->status = 1;
		}
		else {
			// Error
			(mp_data + m_count)->status = 2;
		}

		if (a_cur_pos_check == 1 && !((m_count + 1) % X_COUNT)) {
			MoveCurrentPos();
		}

		RedrawData();
		m_count++;
		Invalidate(FALSE);
		return 1;
	}
	return 0;
}

void ProgressStatusWnd::RedrawData()
{
	ProgressData * p = mp_data + m_count;

	if (p->status == 1) {
		m_draw_dc.SelectObject(&m_normal_brush);
		m_draw_dc.SelectObject(&m_normal_pen);
	}
	else {
		m_draw_dc.SelectObject(&m_error_brush);
		m_draw_dc.SelectObject(&m_error_pen);
	}

	int scroll_pos = GetScrollPos(SB_VERT);
	int y = m_count / X_COUNT - scroll_pos, x = m_count % X_COUNT, fix_y = 0;

	if (scroll_pos == (Y_COUNT - m_display_y_count)) {
		CRect r;
		GetClientRect(r);
		if (r.Height() % HEIGHT) fix_y = HEIGHT - r.Height() % HEIGHT;
	}

	m_draw_dc.Rectangle(x * WIDTH, y * HEIGHT-fix_y, (x + 1)*WIDTH, (y + 1)*HEIGHT - fix_y);
}

void ProgressStatusWnd::RedrawAllData()
{
	int scroll_pos = GetScrollPos(SB_VERT), fix_y = 0;
	ProgressData * p = mp_data + scroll_pos * X_COUNT;

	if (scroll_pos == (Y_COUNT - m_display_y_count)) {
		CRect r;
		GetClientRect(r);
		if (r.Height() % HEIGHT) fix_y = 10 - r.Height() % HEIGHT;
	}
	else {
		// 마지막 페이지
		p = mp_data + (Y_COUNT - m_display_y_count) * X_COUNT;
	}

	for (int y = 0; y < m_display_y_count; y++) {
		for (int x = 0; x < X_COUNT; x++) {
			if (p->status == 1) {
				m_draw_dc.SelectObject(&m_normal_brush);
				m_draw_dc.SelectObject(&m_normal_pen);
			}
			else if(p->status==2) {
				m_draw_dc.SelectObject(&m_error_brush);
				m_draw_dc.SelectObject(&m_error_pen);
			}
			else {
				m_draw_dc.SelectObject(&m_no_data_brush);
				m_draw_dc.SelectObject(&m_no_data_pen);
			}

			m_draw_dc.Rectangle(x * WIDTH, y * HEIGHT - fix_y, (x + 1)*WIDTH, (y + 1)*HEIGHT - fix_y);
			p++;
		}
	}
}

void ProgressStatusWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL) {
		ProcessScroll(nSBCode, nPos, SB_VERT);
		RedrawAllData();
		Invalidate(FALSE);
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


void ProgressStatusWnd::ProcessScroll(UINT nSBCode, UINT nPos, int a_scroll_type)
{
	if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK) {
		SetScrollPos(a_scroll_type, nPos);
	}
	else {
		int scroll_pos = GetScrollPos(a_scroll_type), min = 0, max = 0;
		GetScrollRange(a_scroll_type, &min, &max);
		switch (nSBCode) {
		case SB_LINEDOWN:
			if (scroll_pos < max) SetScrollPos(a_scroll_type, ++scroll_pos);
			break;
		case SB_LINEUP:
			if (scroll_pos < max) SetScrollPos(a_scroll_type, --scroll_pos);
			break;
		case SB_PAGEDOWN:
			if (scroll_pos + 100 < max) SetScrollPos(a_scroll_type, scroll_pos + 100);
			break;
		case SB_PAGEUP:
			if (scroll_pos - 100 > min) SetScrollPos(a_scroll_type, scroll_pos - 100);
			break;
		}
	}
}

void ProgressStatusWnd::ResetData()
{
	SetScrollPos(SB_VERT, 0);
	m_count = 0;
	RedrawAllData();
	Invalidate(FALSE);
	memset(mp_data, 0, sizeof(ProgressData)*Y_COUNT*X_COUNT);
}