
// Scribble2View.cpp : implementation of the CScribble2View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Scribble2.h"
#endif

#include "Scribble2Doc.h"
#include "Scribble2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScribble2View

IMPLEMENT_DYNCREATE(CScribble2View, CScrollView)

BEGIN_MESSAGE_MAP(CScribble2View, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
//	ON_WM_MBUTTONUP()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CScribble2View construction/destruction

CScribble2View::CScribble2View() noexcept
{
	// TODO: add construction code here

}

CScribble2View::~CScribble2View()
{
}

BOOL CScribble2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// CScribble2View drawing

void CScribble2View::OnDraw(CDC* pDC)
{
	CScribble2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//get invalidated rectangle of the view, or in the case of
	//printing, the clipping region of the printer DC.
	CRect rectClip;
	CRect rectStroke;
	pDC->GetClipBox(&rectClip);
	//NOTE: CScrollView::OnPaint() will have already adjusted the 
	//viewpoint origin before calling OnDraw(), to reflect the 
	//currently scrolled position


	//The view delegates the drawing of individual strokes to
	//CStroke::DrawStroke().
	CTypedPtrList<CObList, CStroke*>& strokeList = pDoc->m_strokeList;
	POSITION pos = strokeList.GetHeadPosition();
	while (pos != NULL)
		{

		CStroke* pStroke = strokeList.GetNext(pos);
		rectStroke = pStroke->GetBoundingRect();
		if (!rectStroke.IntersectRect(&rectStroke, &rectClip))
			continue;
		pStroke->DrawStroke(pDC);
		}

	if (!pDoc)
		return;


}


// CScribble2View printing

BOOL CScribble2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CScribble2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CScribble2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CScribble2View diagnostics

#ifdef _DEBUG
void CScribble2View::AssertValid() const
{
	CScrollView::AssertValid();
}

void CScribble2View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CScribble2Doc* CScribble2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CScribble2Doc)));
	return (CScribble2Doc*)m_pDocument;
}
#endif //_DEBUG


// CScribble2View message handlers


void CScribble2View::OnLButtonDown(UINT nFlags, CPoint point)
	{
	//pressing the mouse button in the view window starts
	//a new stroke

	//CScrollView changes the viewport origin and mapping mode.
	//It's necessary to convert the point from device coordinates
	//to logical coordinates, such as are stored in the document.
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);						//Device point to logical point

	m_pStrokeCur = GetDocument()->NewStroke();
	m_pStrokeCur->m_pointArray.Add(point);	//Add first point to the new stroke
	SetCapture();							//capture the mouse until button up
	m_ptPrev = point;						//serves as the MoveTo() anchor point
											//for the LineTo() the next point, as
											// the user drags the mouse

	return;

	//skeleton code, unknown use:
	//CScrollView::OnLButtonDown(nFlags, point);
	}

void CScribble2View::OnLButtonUp(UINT nFlags, CPoint point)
	{
	// Mouse button up is only used if the user is currently drawing
	//a new stroke by dragging the captured mouse

	if (GetCapture() != this)				//if this window (view) didnt capture the 
		return;								//mouse, the user isn't drawing in this window

	CScribble2Doc* pDoc = GetDocument();
	CClientDC dc(this);
	
	//CScrollview changes the viewport origin and mapping mode
	//It's necessary to convert the point from device coordinates
	//to logical coordinates, such as are stored in the document
	OnPrepareDC(&dc);						//set up mapping mode and viewpoint origin
	dc.DPtoLP(&point);

	CPen* pOldPen = dc.SelectObject(pDoc->GetCurrentPen());
	dc.MoveTo(m_ptPrev);
	dc.LineTo(point);
	dc.SelectObject(pOldPen);
	m_pStrokeCur->m_pointArray.Add(point);
											//tell the stroke item that we're done adding
	m_pStrokeCur->FinishStroke();			//points to it. It can finish computing bouding rect
	
	pDoc->UpdateAllViews(this, 0L, m_pStrokeCur); //tell the other views that this stroke has 
											//been added so that they can invalidate the
											// stroke's area in their client area

	ReleaseCapture();						//release the mouse capture established
											//at begining of mouse drag
	return;

	//skeleton code, unknown use:
	//CScrollView::OnLButtonUp(nFlags, point);
	}



void CScribble2View::OnMouseMove(UINT nFlags, CPoint point)
	{
	if (GetCapture() != this)				//if this window (view) didnt capture the 
		return;								//mouse, the user isn't drawing in this window

	CClientDC dc(this);
	//CScrollview changes the viewport origin and mapping mode
	//It's necessary to convert the point from device coordinates
	//to logical coordinates, such as are stored in the document
	OnPrepareDC(&dc);						//set up mapping mode and viewpoint origin
	dc.DPtoLP(&point);

	m_pStrokeCur->m_pointArray.Add(point);
	//Draw a line from previous detected point in the mouse
	//drag to the current point

	CPen* pOldPen = dc.SelectObject(GetDocument()->GetCurrentPen());
	dc.MoveTo(m_ptPrev);
	dc.LineTo(point);
	dc.SelectObject(pOldPen);
	m_ptPrev = point;
	return;
	}


void CScribble2View::OnUpdate(CScrollView* /*pSender*/, LPARAM /*lHint*/, CObject* pHint)
	{
	//The document has informed this view that some data has changed

	if (pHint != NULL)
		{
		if (pHint->IsKindOf(RUNTIME_CLASS(CStroke)))
			{
			CStroke* pStroke = (CStroke*) pHint;		//the hint is that a stroke has been added
			CClientDC dc(this);
			OnPrepareDC(&dc);
			CRect rectInvalid = pStroke->GetBoundingRect();//(or changed) so, invalidate its rectangle
			dc.LPtoDP(&rectInvalid);
			InvalidateRect(&rectInvalid);
			return;
			}
		}
	Invalidate();									//we can't interpret the hint so assume
	return;											//that anything might have been updated
	}



void CScribble2View::OnInitialUpdate()
	{
	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
	CScrollView::OnInitialUpdate();

	}
