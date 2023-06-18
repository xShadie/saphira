###################################################################################################
# Network

import app
import chr
import dbg
import net
import snd

import chr
import chrmgr
import background
import player
import playersettingmodule

import ui
import uiphasecurtain

import localeinfo

class PopupDialog(ui.ScriptWindow):

	def __init__(self):
		print "NEW POPUP DIALOG ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.CloseEvent = 0

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE POPUP DIALOG "
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		PythonScriptLoader = ui.PythonScriptLoader()
		PythonScriptLoader.LoadScriptFile(self, "uiscript/popupdialog.py")

	def Open(self, Message, event = 0, ButtonName = localeinfo.UI_CANCEL, x = 0, y = 0):
		if True == self.IsShow():
			self.Close()
		
		self.Lock()
		self.SetTop()
		self.CloseEvent = event
		AcceptButton = self.GetChild("accept")
		AcceptButton.SetText(ButtonName)
		AcceptButton.SetEvent(ui.__mem_func__(self.Close))
		self.GetChild("message").SetText(Message)
		
		if x != 0 or y != 0:
			self.SetPosition(x, y)
		else:
			self.SetCenterPosition()
		
		self.Show()

	def Close(self):

		if False == self.IsShow():
			self.CloseEvent = 0
			return

		self.Unlock()
		self.Hide()

		if 0 != self.CloseEvent:
			self.CloseEvent()
			self.CloseEvent = 0

	def Destroy(self):
		self.Close()
		self.ClearDictionary()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnIMEReturn(self):
		self.Close()
		return True

##
## Main Stream
##
class MainStream(object):
	isChrData=0

	def __init__(self):
		print "NEWMAIN STREAM ----------------------------------------------------------------------------"
		net.SetHandler(self)
		net.SetTCPRecvBufferSize(128*1024)
		net.SetTCPSendBufferSize(4096)
		net.SetUDPRecvBufferSize(4096)

		self.id=""
		self.pwd=""
		self.addr=""
		self.port=0
		self.account_addr=0
		self.account_port=0
		self.slot=0
		self.isAutoSelect=0
		self.isAutoLogin=0

		self.curtain = 0
		self.curPhaseWindow = 0
		self.newPhaseWindow = 0

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE MAIN STREAM "

	def Destroy(self):
		if self.curPhaseWindow:
			self.curPhaseWindow.Close()
			self.curPhaseWindow = 0

		if self.newPhaseWindow:
			self.newPhaseWindow.Close()
			self.newPhaseWindow = 0

		self.popupWindow.Destroy()
		self.popupWindow = 0

		self.curtain = 0

	def Create(self):
		self.CreatePopupDialog()

		self.curtain = uiphasecurtain.PhaseCurtain()

	def SetPhaseWindow(self, newPhaseWindow):
		if self.newPhaseWindow:
			#print "�̹� ���ο� ������� �ٲۻ��¿��� �� �ٲ�", newPhaseWindow
			self.__ChangePhaseWindow()
		self.curtain.isWarp = False
		self.newPhaseWindow=newPhaseWindow

		if self.curPhaseWindow:
			#print "���̵� �ƿ��Ǹ� �ٲ�"
			self.curtain.FadeOut(self.__ChangePhaseWindow)
		else:
			#print "���� �����찡 ���� ���¶� �ٷ� �ٲ�"
			self.__ChangePhaseWindow()

	def __ChangePhaseWindow(self):
		oldPhaseWindow=self.curPhaseWindow
		newPhaseWindow=self.newPhaseWindow
		self.curPhaseWindow=0
		self.newPhaseWindow=0

		if oldPhaseWindow:
			oldPhaseWindow.Close()

		if newPhaseWindow:
			newPhaseWindow.Open()

		self.curPhaseWindow=newPhaseWindow

		if self.curPhaseWindow:
			self.curtain.FadeIn()
		else:
			app.Exit()

	def CreatePopupDialog(self):
		self.popupWindow = PopupDialog()
		self.popupWindow.LoadDialog()
		self.popupWindow.SetCenterPosition()
		self.popupWindow.Hide()


	## SelectPhase
	##########################################################################################
	def SetLogoPhase(self):
		net.Disconnect()

		import introLogo
		self.SetPhaseWindow(introLogo.LogoWindow(self))

	def Go(self):
		pass

	def SetLoginPhase(self):
		net.Disconnect()
		
		import intrologin
		self.SetPhaseWindow(intrologin.LoginWindow(self))

	def SetSelectEmpirePhase(self):
		try:
			import introempire
			self.SetPhaseWindow(introempire.SelectEmpireWindow(self))
		except:
			import exception
			exception.Abort("networkmodule.SetSelectEmpirePhase")

	def SetReselectEmpirePhase(self):
		try:
			import introempire
			self.SetPhaseWindow(introempire.ReselectEmpireWindow(self))
		except:
			import exception
			exception.Abort("networkmodule.SetReselectEmpirePhase")

	def SetSelectCharacterPhase(self):
		try:
			localeinfo.LoadLocaleData()
			self.popupWindow.Close()
			import introselect
			self.SetPhaseWindow(introselect.SelectCharacterWindow(self))
		except:
			import exception
			exception.Abort("networkmodule.SetSelectCharacterPhase")

	def SetCreateCharacterPhase(self):
		try:
			import introcreate
			self.SetPhaseWindow(introcreate.CreateCharacterWindow(self))
		except:
			import exception
			exception.Abort("networkmodule.SetCreateCharacterPhase")

	def SetTestGamePhase(self, x, y):
		try:
			import introloading
			loadingPhaseWindow=introloading.LoadingWindow(self)
			loadingPhaseWindow.LoadData(x, y)
			self.SetPhaseWindow(loadingPhaseWindow)
		except:
			import exception
			exception.Abort("networkmodule.SetLoadingPhase")

	def SetLoadingPhase(self, isWarp = False):
		try:
			import introLoading
			self.SetPhaseWindow(introLoading.LoadingWindow(self, isWarp))
			self.curtain.isWarp = True
		except:
			import exception
			exception.Abort("networkModule.SetLoadingPhase")

	def SetGamePhase(self):
		try:
			import game
			self.popupWindow.Close()
			self.SetPhaseWindow(game.GameWindow(self))
		except:
			raise
			import exception
			exception.Abort("networkmodule.SetGamePhase")

	################################
	# Functions used in python

	## Login
	def Connect(self):
		import constinfo
		if constinfo.KEEP_ACCOUNT_CONNETION_ENABLE:
			net.ConnectToAccountServer(self.addr, self.port, self.account_addr, self.account_port)
		else:
			net.ConnectTCP(self.addr, self.port)

		#net.ConnectUDP(IP, Port)

	def SetConnectInfo(self, addr, port, account_addr=0, account_port=0):
		self.addr = addr
		self.port = port
		self.account_addr = account_addr
		self.account_port = account_port

	def GetConnectAddr(self):
		return self.addr

	def SetLoginInfo(self, id, pwd):
		self.id = id
		self.pwd = pwd
		net.SetLoginInfo(id, pwd)

	def CancelEnterGame(self):
		pass

	## Select
	def SetCharacterSlot(self, slot):
		self.slot=slot

	def GetCharacterSlot(self):
		return self.slot

	## Empty
	def EmptyFunction(self):
		pass
