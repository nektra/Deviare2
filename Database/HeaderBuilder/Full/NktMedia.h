HRESULT ReadSample(DWORD dwStreamIndex, DWORD dwControlFlags, DWORD *pdwActualStreamIndex, DWORD *pdwStreamFlags, LONGLONG *pllTimestamp, HANDLE **ppSample);
HRESULT OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, HANDLE *pSample);
HRESULT RenderStream(const GUID *pCategory, const GUID *pType, HANDLE *pSource, HANDLE *pIntermediate, HANDLE *pSink);


