
// Scribble2Doc.cpp : implementation of the CScribble2Doc class
//

#include "pch.h"
#include "framework.h"
#include "CPenWidthsDlg.h"

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Scribble2.h"
#endif

#include "Scribble2Doc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CScribble2Doc

IMPLEMENT_DYNCREATE(CScribble2Doc, CDocument)

BEGIN_MESSAGE_MAP(CScribble2Doc, CDocument)
	ON_COMMAND(ID_FILE_SEND_MAIL, &CScribble2Doc::OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, &CScribble2Doc::OnUpdateFileSendMail)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, &CScribble2Doc::OnEditClearAll)
	ON_COMMAND(ID_PEN_THICK_OR_THIN, &CScribble2Doc::OnPenThickOrThin)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, &CScribble2Doc::OnUpdateEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_PEN_THICK_OR_THIN, &CScribble2Doc::OnUpdatePenThickOrThin)
	ON_COMMAND(ID_PEN_WIDTHS, &CScribble2Doc::OnPenWidths)
END_MESSAGE_MAP()


// CScribble2Doc construction/destruction

CScribble2Doc::CScribble2Doc() noexcept
{
	// TODO: add one-time construction code here

}

CScribble2Doc::~CScribble2Doc()
{
}

BOOL CScribble2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	InitDocument();

	// (SDI documents will reuse this document)

	return TRUE;
}




// CScribble2Doc serialization

void CScribble2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_sizeDoc;								//stores size of window for scrolling

	}
	else
	{
		ar >> m_sizeDoc;

	}
	m_strokeList.Serialize(ar);
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CScribble2Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CScribble2Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CScribble2Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CScribble2Doc diagnostics

#ifdef _DEBUG
void CScribble2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CScribble2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CScribble2Doc commands


CStroke* CScribble2Doc::NewStroke()
	{
	CStroke* pStrokeItem = new CStroke(m_nPenWidth);
	m_strokeList.AddTail(pStrokeItem);
	SetModifiedFlag();						//mark document as modified
	return pStrokeItem;
	}


void CScribble2Doc::InitDocument()
	{
	m_bThickPen = FALSE;		
	m_nThinWidth = 2;							
	m_nThickWidth = 5;	
	ReplacePen();							//initilise pen according to current width
	m_sizeDoc = CSize(800, 900);			//default document size in screen pixels
							
	//code from start of tutorial replaced in chapter 7
	//m_nPenWidth = 2;											//default 2 pixel pen width
	//m_penCur.CreatePen(PS_SOLID, m_nPenWidth, RGB(0, 0, 0));	// solid black pen
	}


BOOL CScribble2Doc::OnOpenDocument(LPCTSTR lpszPathName)
	{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	InitDocument();
	return TRUE;
	}


void CScribble2Doc::DeleteContents()
	{
	while (!m_strokeList.IsEmpty())
		{
		delete m_strokeList.RemoveHead();
		}
	}

IMPLEMENT_SERIAL(CStroke, CObject, 2);

//Constructor declares protected in ScribbleDoc.h is used only by app framework during serialization
CStroke::CStroke() {}

//Constructer for public use, relies on CObject for destructor
CStroke::CStroke(UINT nPenWidth)
	{
	m_nPenWidth = nPenWidth;
	m_rectBounding.SetRectEmpty();						//initialise an empty rectangle
	}

void CStroke::Serialize(CArchive& ar)
	{
	if (ar.IsStoring())
		{
		ar << (WORD)m_nPenWidth;
		ar << m_rectBounding;							//stores size of altered draw rectangle
		m_pointArray.Serialize(ar);
		}
	else
		{
		WORD w;
		ar >> w;
		ar >> m_rectBounding;							//reads member variable from archive
		m_nPenWidth = w;
		m_pointArray.Serialize(ar);
		}
	}

BOOL CStroke::DrawStroke(CDC* pDC)
	{
	CPen penStroke;
	if (!penStroke.CreatePen(PS_SOLID, m_nPenWidth, RGB(0, 100, 102)))
		return FALSE;
	CPen* pOldPen = pDC->SelectObject(&penStroke);
	pDC->MoveTo(m_pointArray[0]);
	for (int i = 1; i < m_pointArray.GetSize(); i++)
		{
		pDC->LineTo(m_pointArray[i]);
		}
	pDC->SelectObject(pOldPen);
	return TRUE;
	}

void CScribble2Doc::OnEditClearAll()
	{
	DeleteContents();
	SetModifiedFlag();
	UpdateAllViews(NULL);
	}


void CScribble2Doc::OnPenThickOrThin()
	{
	//toggle the state of the pen between thin and thick
	m_bThickPen = !m_bThickPen;

	//change the current pen to reflect the new width
	ReplacePen();
	}


// initialise pen according to current width
void CScribble2Doc::ReplacePen()
	{
	m_nPenWidth = m_bThickPen ? m_nThickWidth : m_nThinWidth;
	//change the current pen to reflect the new width
	m_penCur.DeleteObject();
	m_penCur.CreatePen(PS_SOLID, m_nPenWidth, RGB(0, 102, 102));
	}


void CScribble2Doc::OnUpdateEditClearAll(CCmdUI* pCmdUI)
	{
	//Enable the user-interface object (menu item or tool -
	//bar button) if the document is non-empty, i.e., has 
	//at least one stroke
	pCmdUI->Enable(!m_strokeList.IsEmpty());
	}


void CScribble2Doc::OnUpdatePenThickOrThin(CCmdUI* pCmdUI)
	{
	//add check mark to pen thick line menu item if teh current
	//pen width is "thick"
	pCmdUI->SetCheck(m_bThickPen);
	}


void CScribble2Doc::OnPenWidths()
	{
	CPenWidthsDlg dlg;						//initialise dialogue
	dlg.m_nThinWidth;
	dlg.m_nThickWidth;

	if (dlg.DoModal() == IDOK)				//retrieve the dialoge box
		{
		m_nThinWidth = dlg.m_nThinWidth;	//retrieve the data
		m_nThickWidth = dlg.m_nThickWidth;
		}

	ReplacePen();							//update the pen used by views when 
											//drawing new strokes to reflect the new
											//pen widths for "thick" and "thin"

	}


void CStroke::FinishStroke()
//calculates the bounding rectangle for a stroke
	{
	if (m_pointArray.GetSize() == 0)
		{
		m_rectBounding.SetRectEmpty();
		return;
		}
	CPoint pt = m_pointArray[0];
	m_rectBounding = CRect(pt.x, pt.y, pt.x, pt.y);
	
	for (int i = 1; i < m_pointArray.GetSize(); i++)
		{
		//if the point lies outside the accumulated bounding
		//rectangle, then inflate the bouding rect to include it
		pt = m_pointArray[i];
		m_rectBounding.left = min(m_rectBounding.left, pt.x);
		m_rectBounding.right = max(m_rectBounding.right, pt.x);
		m_rectBounding.top = min(m_rectBounding.top, pt.y);
		m_rectBounding.bottom = max(m_rectBounding.bottom, pt.y);
		}

	// add the pen width to the bounding rectangle. This is needed
	// to account for the width of the stroke when invalidating
	// the screen.
	m_rectBounding.InflateRect(CSize(m_nPenWidth, m_nPenWidth));
	return;
	}
