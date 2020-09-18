
// Scribble2View.h : interface of the CScribble2View class
//

#pragma once


class CScribble2View : public CScrollView
{
protected: // create from serialization only
	CScribble2View() noexcept;
	DECLARE_DYNCREATE(CScribble2View)

// Attributes
public:
	CScribble2Doc* GetDocument() const;

protected:
	CStroke* m_pStrokeCur;
	CPoint m_ptPrev;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CScribble2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnUpdate(CScrollView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnInitialUpdate();
	};

#ifndef _DEBUG  // debug version in Scribble2View.cpp
inline CScribble2Doc* CScribble2View::GetDocument() const
   { return reinterpret_cast<CScribble2Doc*>(m_pDocument); }
#endif

