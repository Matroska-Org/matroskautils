VERSION 5.00
Begin VB.Form Form1 
   AutoRedraw      =   -1  'True
   Caption         =   "Form1"
   ClientHeight    =   4080
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5955
   LinkTopic       =   "Form1"
   ScaleHeight     =   4080
   ScaleWidth      =   5955
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "Command1"
      Height          =   495
      Left            =   4680
      TabIndex        =   0
      Top             =   3480
      Width           =   1215
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Declare Function CreateParserObject Lib "MatroskaProp.dll" (parserObj As Long, ByVal filename As String) As Long
Private Declare Function FillParserObject Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal parseTags As Boolean, ByVal parseWholeFile As Boolean) As Long
Private Declare Function DeleteParserObject Lib "MatroskaProp.dll" (ByVal parserObj As Long) As Long

Private Declare Function GetTrackCount Lib "MatroskaProp.dll" (ByVal parserObj As Long) As Long
Private Declare Function GetTrackUID Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Long) As Long
Private Declare Function GetTrackType Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Long) As Long
Private Declare Function GetTrackTypeLen Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Long) As Long
Private Declare Function GetTrackTypeStr Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Long, ByVal track_type_name As String) As Long
Private Declare Function GetTrackCodecIDLen Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Long) As Long
Private Declare Function GetTrackCodecIDStr Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Long, ByVal codec_name As String) As Long
Private Declare Function GetTrackLength Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Long) As Long

'Private Declare Function GetTrackAudioInfo Lib "E:\windows\system32\shellext\MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Long, ByVal audio_track_info As MatroskaAudioTrackInfo) As Long
Private Declare Function SetAR Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Integer, ByVal display_x As Integer, ByVal display_y As Integer) As Long
Private Declare Function UpdateTrackLanguage Lib "MatroskaProp.dll" (ByVal parserObj As Long, ByVal track_no As Integer, ByVal lang As String) As Long

Private Sub Command1_Click()
    
    Dim par As Long
    Dim ps As Long
    
    Print "Parser", "Track No", "Return value", "Return Value2"
    
    ps = CreateParserObject(par, "C:\test.mkv")
    Print Hex$(par), , ps
    
    ps = FillParserObject(par, False, True)
    Print Hex$(par), , ps
    
    Dim track_count As Long
    track_count = GetTrackCount(par)
    Print Hex$(par), ps
    For t = 0 To track_count
        Dim t_uid As Long
        ps = GetTrackType(par, t_uid)
        Print Hex$(par), ps, t, t_uid
        ps = GetTrackLength(par, t)
        Print Hex$(par), ps, t, t_uid
        
        Dim test As String
        Dim str_len As Integer
        str_len = GetTrackTypeLen(par, t)
        If str_len > -1 Then
            test = String$(str_len, " ")
            ps = GetTrackTypeStr(par, t, test)
            Print Hex$(par), ps, t, test
        End If
        
        Dim codecID As String
        str_len = GetTrackCodecIDLen(par, t)
        If str_len > -1 Then
            codecID = String$(str_len, " ")
            ps = GetTrackCodecIDStr(par, t, codecID)
            Print Hex$(par), ps, t, codecID
        End If
        Print Hex$(par), ps, t, GetTrackLength(par, t)
        'Dim audio_info As MatroskaAudioTrackInfo
        'audio_info.audio_SampleRate = 11050
        'audio_info.audio_BitDepth = 16
        'audio_info.audio_Channels = 2
        'ps = GetTrackAudioInfo(par, t, audio_info)
        'Print Hex$(par), ps, t, audio_info.audio_SampleRate + " " + audio_info.audio_Channels + " " + audio_info.audio_BitDepth
    Next t
    'ps = SetAR(par, 1, 640, 480)
    'Print Hex$(par), ps
    
    ps = DeleteParserObject(par)
    Print Hex$(par), ps
End Sub

