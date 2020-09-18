
// Scribble2Doc.h : interface of the CScribble2Doc class
//


#pragma once

//Forward declaration of data structure class
class CStroke;

class CScribble2Doc : public CDocument
{
// Attributes
public:
	CTypedPtrList<CObList, CStroke*> m_strokeList;			//m_strokeList is an template instance
	CPen* GetCurrentPen() { return &m_penCur; }				//GetCurrentPen is inline so defined in .h
	
protected:
	CSize m_sizeDoc;

// Operations
public:
	CSize GetDocSize() { return m_sizeDoc; }

protected: // create from serialization only
	CScribble2Doc() noexcept;
	DECLARE_DYNCREATE(CScribble2Doc)

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CScribble2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
	// The current width of the line drawn by the pen
	UINT m_nPenWidth;
	BOOL m_bThickPen;							//thick currently selected or not
	UINT m_nThinWidth;							//current definition of thin
	UINT m_nThickWidth;							//current definition of thick

	CPen m_penCur;					// a CPen object used to do the drawing. Main attribute width.
									//Pen is created when the document is constructed 
									//and is used during the creation of new strokes.
	
public:
	CStroke* NewStroke();
protected:
	void InitDocument();
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	afx_msg void OnEditClearAll();
	afx_msg void OnPenThickOrThin();
protected:
	// initialise pen according to current width
	void ReplacePen();
public:
	afx_msg void OnUpdateEditClearAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePenThickOrThin(CCmdUI* pCmdUI);
	afx_msg void OnPenWidths();
	};

//class cStroke
//a stroke is a series of connected points in the Scribble drawing.
//a scribble document may have multiple strokes
class CStroke : public CObject
	{
	//Attributes
	protected:
		UINT m_nPenWidth;							//one width applies to entire stroke
		CRect m_rectBounding;						//smallest rect that surrounds all of
													//the points in the stroke

	public:
		CArray<CPoint, CPoint> m_pointArray;		//series of connected points defining stroke

	//operations
	public:
		CStroke(UINT nPenWidth);
		BOOL DrawStroke(CDC* pDC);
		virtual void Serialize(CArchive& ar);

		CRect& GetBoundingRect() { return m_rectBounding; }
	
	protected:
		CStroke();
		DECLARE_SERIAL(CStroke)
	public:
		void FinishStroke();
	};

