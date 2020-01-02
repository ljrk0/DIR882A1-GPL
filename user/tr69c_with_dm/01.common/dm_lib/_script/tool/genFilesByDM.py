import sys
import os
import re
import xlrd
import logging
import xml.dom.minidom
import time

#----------------------------------------------------------------------------------------------------------------------
# minidom customize
#----------------------------------------------------------------------------------------------------------------------
def fixed_writexml(self, writer, indent="", addindent="", newl=""):
    writer.write(indent+"<" + self.tagName)

    attrs = self._get_attributes()
    a_names = attrs.keys()
    a_names.sort()

    for a_name in a_names:
        writer.write(" %s=\"" % a_name)
        xml.dom.minidom._write_data(writer, attrs[a_name].value)
        writer.write("\"")

    if self.childNodes:
        if len(self.childNodes) == 1 and self.childNodes[0].nodeType == xml.dom.minidom.Node.TEXT_NODE:
            writer.write(">")
            self.childNodes[0].writexml(writer, "", "", "")
            writer.write("</%s>%s" % (self.tagName, newl))
            return
        writer.write(">%s"%(newl))
        for node in self.childNodes:
            if node.nodeType is not xml.dom.minidom.Node.TEXT_NODE:
                node.writexml(writer,indent+addindent,addindent,newl)
        writer.write("%s</%s>%s" % (indent,self.tagName,newl))
    else:
        writer.write("/>%s"%(newl))

xml.dom.minidom.Element.writexml = fixed_writexml

#----------------------------------------------------------------------------------------------------------------------
# tool body
#----------------------------------------------------------------------------------------------------------------------
# configuration : Tool
ExcelFileDir = "./excel_files"
CommonSrcDir = "../../../"
ResultSrcDir = "../../"
ResultDbLibDir = "../../lib/"
XmlWithComment = "yes"

# configuration : Dbconfig
SelfDefinedPrefix = ""
WorkingDir = '/tmpfs/'
LoadingDir = '/data/'
BackupDir = '/otherdata/'
MappingXmlDir = '/APP/'

def parseConfiguration(confFile):
    sectionName = ""
    result = False

    try:
        with open(confFile, 'rb') as f:
            for line in f.readlines():
                line = line.strip()

                if line.startswith("[") and line.endswith("]"):
                    sectionName = line[1:len(sectionName)-1].strip().lower()
                    if not sectionName in ("tool", "db"):
                        logger.debug("Invalid configure section ( " + sectionName + " )")
                elif line.startswith("#"):
                    continue    # skip comment line
                else:
                    # skip blank line.
                    if line == "" :
                        continue

                    # if error, skip it
                    if "=" not in line:
                        logger.debug("Invalid configure item ( " + line + " )")
                        continue

                    # parse each item
                    if sectionName in ("tool", "db") :
                        result = True

                        (key, value) = line.split("=")
                        key = key.strip()
                        value = value.strip()
                        if value != "" :
                            globals()[key] = value   # set a global varibale
                    else:
                        logger.debug("configure item (" + line + ") under an invalid section=" + sectionName + "")

    except Exception as e:
        logger.debug("Exception occurred : " + str(e) )
        logger.debug("failed to get information from " + confFile + ", use default ")

    return result

def printConfig() :
    logger.debug("-------------- configuration ------------")
    logger.debug(": [Tool]")
    logger.debug(":    ExcelFileDir=" + str(ExcelFileDir))
    logger.debug(":    CommonSrcDir=" + CommonSrcDir)
    logger.debug(":    ResultSrcDir=" + ResultSrcDir)
    logger.debug(":    ResultDbLibDir=" + ResultDbLibDir)
    logger.debug(":    XmlWithComment=" + XmlWithComment )
    logger.debug(": ")
    logger.debug(": [Db]")
    logger.debug(":    SelfDefinedPrefix=" + SelfDefinedPrefix)
    logger.debug(":    WorkingDir=" + WorkingDir)
    logger.debug(":    LoadingDir=" + LoadingDir)
    logger.debug(":    BackupDir=" + BackupDir)
    logger.debug(":    MappingXmlDir=" + MappingXmlDir)
    logger.debug("-------------- configuration ------------")

def calcRelativePath(referPath, objectPath) :
    referPathList = os.path.abspath(os.path.normpath(referPath)).split("/")
    objectPathList = os.path.abspath(os.path.normpath(objectPath)).split("/")

    sameDepthLevel = 0
    for curLevelName in referPathList :
        if sameDepthLevel >= len(objectPathList) :
            break
        elif curLevelName == objectPathList[sameDepthLevel]:
            sameDepthLevel = sameDepthLevel + 1
        else :
            break

    if sameDepthLevel > 1 :
        # same path is not root
        referPathList = referPathList[sameDepthLevel:]

        if sameDepthLevel + 1 <= len(objectPathList) :
            objectPathList = objectPathList[sameDepthLevel:]
        else:
            objectPathList = []

        result = "../" * len(referPathList)
        if len(result) == 0:
            result = "./"
        if len(objectPathList) > 0 :
            result = result + reduce(lambda x,y : x + "/" + y, objectPathList)
    elif sameDepthLevel == 1:
        # same path is root
        result = objectPath
    else :
        # Exception case
        result = objectPath
        logger.debug("== Invalid path ==")

    return result

def svnRemoveLocalfiles(path):
    if os.path.isfile(path):
        try:
            os.remove(path)
        except:
            pass
    elif os.path.isdir(path):
        for item in os.listdir(path):
            itempath=os.path.join(path,item)
            svnRemoveLocalfiles(itempath)
        try:
            os.rmdir(path)
        except:
            pass

class loggerManager:
    def __init__(self):
        self.logfile = os.getcwd().strip()+'/log.txt'
    def debug(self,logstring):
        currentTime = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
        currentFunc = sys._getframe().f_back.f_code.co_name
        currentLine = sys._getframe().f_back.f_lineno
        logToWrite = str(currentTime)+' '+str(currentFunc)+' '+str(currentLine)+' '+logstring

        print(logToWrite)

        fw = open(self.logfile,'a')
        fw.write(logToWrite+'\n')
        fw.close()

logger = loggerManager()


class ModuleItem:
    """
    the class fot the first and second lines of the excel content
    it manage the module items
    ---------------------------------------------------------
    moduleTypedict      : defined at the first line and contain the module number for each type
    startCol            : the first module column
    accessModuleCnt     : access module count
    accessModuleList    : access module name list
    accessModuledict    : dict access module
    getModuleCnt        : get module count
    getModuleList       : get module name list
    getModuledict       : dict get module
    notifyModuleCnt     : notify module count
    notifyModuleList    : notify module name list
    notifyModuledict    : dict notify module
    ---------------------------------------------------------
    """

    def __init__(self):
        self.accessModuleStartCol=0
        self.accessModuleList=[]
        self.accessModuledict={}

        self.getModuleStartCol=0
        self.getModuleList=[]

        self.setModuleStartCol=0
        self.setModuleList=[]

        self.notifyModuleStartCol=0
        self.notifyModuleList=[]


    #parse header line, and create the module list
    def parseModule(self, lineArr0, lineArr1):

        tmpList0=lineArr0
        tmpList1=lineArr1

        columnCnt = 0
        for i in range(0,len(tmpList1)):
            if tmpList1[i].strip()=='':
                break
            else:
                columnCnt = columnCnt + 1

        logger.debug('column length is '+str(columnCnt))

        #create module list : Access Right / Get Hooker / Notify Callback
        self.getModuleList=[]
        self.notifyModuleList=[]
        self.accessModuleList=[]

        #logger.debug("Get model default length "+str(len(self.getModuleList)))

        for i in range(0, columnCnt):
            if tmpList0[i] == 'GetHooker':
                self.getModuleStartCol = i
            elif tmpList0[i] == 'SetHooker':
                self.setModuleStartCol = i
            elif tmpList0[i] == 'NotifyCallback':
                self.notifyModuleStartCol = i
            elif tmpList0[i] == 'AccessRight':
                self.accessModuleStartCol = i


        for i in range(self.getModuleStartCol, self.setModuleStartCol):
            self.getModuleList.append(str(tmpList1[i]).strip().replace(" ","_"))

        for i in range(self.setModuleStartCol, self.notifyModuleStartCol):
            self.setModuleList.append(str(tmpList1[i]).strip().replace(" ","_"))

        for i in range(self.notifyModuleStartCol, self.accessModuleStartCol):
            self.notifyModuleList.append(str(tmpList1[i]).strip().replace(" ","_"))

        for i in range(self.accessModuleStartCol, columnCnt):
            self.accessModuleList.append(str(tmpList1[i]).strip().replace(" ","_"))
            self.accessModuledict[str(tmpList1[i]).strip().replace(" ","_")]=2**(i-self.accessModuleStartCol)


        #logger.debug("Get model new length "+str(len(self.getModuleList)))
        #logger.debug("self.getModuleStartCol = "+str(self.getModuleStartCol))
        #logger.debug("self.setModuleStartCol = "+str(self.setModuleStartCol))
        #logger.debug("self.notifyModuleStartCol = "+str(self.notifyModuleStartCol))
        #logger.debug("self.accessModuleStartCol = "+str(self.accessModuleStartCol))

class PatternItem:
    """
    the class of one line excel content,
    one line will be mapped to one item
    but the item also has additional info
    the excel columns is list below
    ---------------------------------------------------------
    name     : the name of item, if an object, will be end with "."
                   the object name "a.{i}.b.{i}.c."
                   the item name    "c"
    type     : the type of item, will be used in PatternTable
    C type   : the C language type of item, will be used in profile.
    funGenFlg    : the flag for function generation, 1:add, 2: get, 4: set, 8: del,
                   the flag may use & operator to generate multiple function, 3: add+get
    Value range  : value range of the item,
                          if object type, will be the max number of array,
                          if  array type, will be the number of array, [num]
    Default Value: the default value of item

                          if object type, will be the number of array, [num]
    R module Bitmap     : the module Bitmap which module can read the correspond item.
    W module Bitmap     : the module Bitmap which module can write the correspond item.
    GetHandler dict     : the handler which module can get the correspond item.
    NotifyHandler dict  : the handler which module will be called, when the value was changed on the correspond item or object.

    description  : currently not used. maybe used as comment
    ------------------------------------------------------------------
    """
    maxLevel=-1        #the max level of object, only used to judge the sub node or parent node
    parentFullName=""          #record last parent full name

    def __init__(self):
        """
        to initialize all the member of class
        """
        self.orgName=""                                                         #the orginal name of node generately "a.b.c.{i}.d"
        self.orgNameReplace=""                                                  #the orginal name of node generately "a.b.c.{i}.d"
        self.nodeFullName=""                                                    #node full name, generately "a.b.c.d", removed all {i}
        self.nodeFullNameReplace=""                                             #node full name, generately "a.b.c.d", removed all {i}, replace original string to particular string
        self.parentFullName=""                                                  #parent name's full name, "a.b.c"
        self.name=""                                                            #name of node, generally the last word of nodeFullName "d"
        self.nameReplace=""                                                     #name of node, generally the last word of nodeFullName "d", replace original string to particular string
        self.definition=""                                                      #Item definition as XXX_ID
        self.struction=""                                                       #Item struction as XXX_T

        self.description=""                                                     #description
        self.selfdefined=""                                                     #self-defined parameters. if it is 1, this parameter name should be added prefix str.

        self.type=""                                                            #type of node
        self.cType=""                                                           #the type for c struct

        self.pToStringFunc=""                                                   #callback function to convert ctype to string
        self.pToStructFunc=""                                                   #callback function to convert string to ctype

        self.genOperationFlg = ""                                               #the operation generation flag, 0: nothing, 1:add 2:get 4:set 8: del
        self.defaultValue=""                                                    #default value
        self.supportedXmlAttrs=""                                               #supported xml attributes
        self.defaultXmlAttrValue=""                                             #default xml attribute value
        self.XmlAttrNotifyEntities=""                                           #xml attribute-notify entities
        self.valueRange=""                                                      #the value range of node
        self.AutoSaved=0                                                        #the auto saved flag
        self.upgradeTransfer=0                                                  #the upgrade-transfer flag
        self.ResetTransfer=0                                                    #the factory-reset flag
        self.bootAction=""                                                      #the boot-action

        self.pValidityCheckHandler=""                                           #ValidityCheck

        self.UseOperatorDefault=0                                               #use Operator Default Value
        self.UseHardwareDefault=0                                               #use Hardware Default Value
        self.forceActiveNotification=0                                          #force active notification
        self.Tr69Password=0                                                     #if 1, acs will get emtpy value always.

        self.AsVendorConfig=0                                                   #As vendor config parameter
        self.AsKeyParam=0                                                       #As key parameter in vendor config file

        self.parentIndex=-1                                                     #parent index
        self.selfIndex=-1                                                       #index of self
        self.siblingIndex=-1                                                    #index of sibling
        self.firstChildIndex=-1                                                 #index of first child
        self.childrenCounter=-1                                                 #counter of children

        self.isObject=False                                                     #if the node is object, True
        self.isMultObj = False                                                  #if the node is multiple object, True
        self.isSimpleArray=False                                                #if the node is an array of simple type, such as int, long
        self.objLevel=-1                                                        #the level of object, if not object, will be -1

        self.readBitmap = 0                                                     #bitmap: module can read
        self.writeBitmap = 0                                                    #bitmap: module can write
        self.getHandlerList = {}                                                #get hooker
        self.setHandlerList = {}                                                #set hooker
        self.notifyHandlerlist  = {}                                            #list: notify callback list

        self.POSMap={                                                           #define the column number
                     "name":                        0,
                     "selfdefined":                 2,
                     "description":                 3,
                     "type":                        4,
                     "ctype":                       5,
                     "funcGenFlg":                  6,
                     "valueRange":                  7,
                     "defaultValue":                8,
                     "supportedXmlAttrs":           9,
                     "defaultXmlAttrValue":         10,
                     "XmlAttrNotifyEntities":       11,
                     "AutoSaved":                   12,
                     "UpgradeTransfer":             13,
                     "ResetTransfer":               14,
                     "BootAction":                  15,
                     "validityCheck":               16,
                     "UseOperatorDefault":          17,
                     "UseHardwareDefault":          18,
                     "forceActiveNotification":     19,
                     "Tr69Password":                20,
                     "AsVendorConfig":              21,
                     "AsKeyParam":                  22
                     }

        self.operationFunc={                                                    #remember the operation function if exist
                    "add":              "",
                    "get":              "",
                    "set":              "",
                    "del":              ""
                    }

    def __str__(self):
        return unicode(self).encode('utf-8')

    def __unicode__(self):
        return "orgName: "+self.orgName

    def processName(self, externConvertList, convertToString, convertToStruct):
        """
         if the name endswith ".",  get the parent name from the "a.b.c" structure
         else, get the parent name from the global __class__.name
         this operation will set the isObject, maxLevel, name, parentFullName, isSimpleArray
        """
        #process object name
        if(self.nodeFullName.endswith('.')):
            self.isObject = True
            self.objLevel = self.nodeFullName.count(".")
            if (self.__class__.maxLevel < self.objLevel):
                self.__class__.maxLevel = self.objLevel

            tmpList = self.nodeFullName.split(".")
            self.name=tmpList[len(tmpList)-2].strip()
            self.__class__.parentFullName=self.nodeFullName.strip()
            rIndex=0
            if self.nodeFullName[0:-2].rfind('.') == -1:
                rIndex=len(self.nodeFullName)
            else:
                rIndex=self.nodeFullName[0:-2].rfind('.')+1
            self.parentFullName=self.nodeFullName[0:rIndex]

            if self.orgName.endswith("{i}."):
                self.isMultObj = True

            self.definition = self.nodeFullName.replace(".", "_").upper() + "ID"
            self.struction = self.nodeFullName.replace(".","_").upper()+ "T"
        else:
            self.name=self.nodeFullName.strip()
            self.parentFullName=self.__class__.parentFullName
            self.nodeFullName = self.parentFullName + self.name
            self.definition = self.nodeFullName.replace(".", "_").upper() + "_ID"
            self.struction = self.cType.replace("[]","")


        #get pToStringFunc by cType
        self.pToStringFunc = convertToString[self.cType]
        #if self.cType == "UINT32_BITMAP":
        #    self.pToStringFunc = self.nodeFullName.replace(".", "_") + "_DmConvertBitmapToString"
        #    externConvertList.append("extern BOOL8 "+self.pToStringFunc+"(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue);")
        #if self.cType == "UINT32_ENUM":
        #    self.pToStringFunc = self.nodeFullName.replace(".", "_") + "_DmConvertEnumToString"
        #    externConvertList.append("extern BOOL8 "+self.pToStringFunc+"(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue);")

        #get pToStructFunc by cType
        self.pToStructFunc = convertToStruct[self.cType]
        #if self.cType == "UINT32_BITMAP":
        #    self.pToStructFunc = self.nodeFullName.replace(".", "_") + "_DmConvertStringToBitmap"
        #    externConvertList.append("extern BOOL8 "+self.pToStructFunc+"(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue);")
        #if self.cType == "UINT32_ENUM":
        #    self.pToStructFunc = self.nodeFullName.replace(".", "_") + "_DmConvertStringToEnum"
        #    externConvertList.append("extern BOOL8 "+self.pToStructFunc+"(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue);")


    #parse one line of csv, and fill the item's content
    def parseLine(self, lineArr, moduleItem, externConvertList, convertToString, convertToStruct):
        """
        the lineArr is an arrya of csv line
        """
        #logger.debug(lineArr)
        tmpList=lineArr
        columnCnt = len(tmpList)
        if columnCnt >= self.POSMap['name']+1 :
            #get the node name
            self.orgName = tmpList[self.POSMap['name']]
            #logger.debug(self.orgName)
            try:
                self.nodeFullName = tmpList[self.POSMap['name']].replace(".{i}", "")
            except:
                self.nodeFullName = ""

            # check if self-defined
            self.selfdefined = str(tmpList[self.POSMap['selfdefined']]).strip()

            #get description
            self.description = str(tmpList[self.POSMap['description']]).replace('"','\\"').replace("\n", " ").replace("\r", "")

            #get the data type
            if columnCnt >=self.POSMap['type']+1:
                self.type= tmpList[self.POSMap['type']].strip()

            #get the c type of node, will be used in c profile file
            if columnCnt >=self.POSMap['ctype']+1:
                try:
                    self.cType = tmpList[self.POSMap['ctype']].strip()
                    if self.cType == "":
                        self.cType = "STRUCT"
                except:
                    self.cType= "unknown"
                if self.cType.strip().endswith(']'):
                    self.isSimpleArray = True

            #get the funcGenFlg
            if columnCnt >=self.POSMap['funcGenFlg']+1:
                try:
                    if str(tmpList[self.POSMap['funcGenFlg']]).strip() == "":
                        self.genOperationFlg = "GS"
                    else:
                        self.genOperationFlg=tmpList[self.POSMap['funcGenFlg']]
                except:
                    self.genOperationFlg = "GS"

            #get the value range
            if columnCnt >=self.POSMap['valueRange']+1:
                try:
                    self.valueRange=int(tmpList[self.POSMap['valueRange']])
                except:
                    self.valueRange=str(tmpList[self.POSMap['valueRange']]).strip()

            #get the default value
            if columnCnt >=self.POSMap['defaultValue']+1:
                #get the default value and change it to int
                if re.search("\.0$", str(tmpList[self.POSMap['defaultValue']])) and not self.cType == "CM_IP_ADDR_T":
                    self.defaultValue = str(tmpList[self.POSMap['defaultValue']]).replace('"','\\"').replace('\n','\\n').replace('\r','')[:-2]
                else:
                    self.defaultValue = str(tmpList[self.POSMap['defaultValue']]).replace('"','\\"').replace('\n','\\n').replace('\r','')

            ##if self.defaultValue == "__reference__":
            ##    self.defaultValue =

            #get supportedXmlAttrs flag
            if columnCnt >=self.POSMap['supportedXmlAttrs']+1:
                try:
                    self.supportedXmlAttrs="\""+str(tmpList[self.POSMap['supportedXmlAttrs']]).strip().replace('"','\\"').replace('\n',',').replace('\r','')+"\""
                except:
                    self.supportedXmlAttrs = "\"\""

            #get defaultXmlAttrValue flag
            if columnCnt >=self.POSMap['defaultXmlAttrValue']+1:
                try:
                    self.defaultXmlAttrValue="\""+str(tmpList[self.POSMap['defaultXmlAttrValue']]).strip().replace('"','\\"').replace('\n','\\n').replace('\r','')+"\""
                except:
                    self.defaultXmlAttrValue = "\"\""

            #get XmlAttrNotifyEntities flag
            if columnCnt >=self.POSMap['XmlAttrNotifyEntities']+1:
                try:
                    self.XmlAttrNotifyEntities="\""+str(tmpList[self.POSMap['XmlAttrNotifyEntities']]).strip().replace('"','\\"').replace('\n',',').replace('\r','')+"\""
                except:
                    self.XmlAttrNotifyEntities = "\"\""

            #get the auto saved flag
            if columnCnt >=self.POSMap['AutoSaved']+1:
                try:
                    self.autoSaved=int(tmpList[self.POSMap['AutoSaved']])
                except:
                    self.autoSaved = "0"

            #get the upgrade transfer flag
            if columnCnt >=self.POSMap['UpgradeTransfer']+1:
                try:
                    self.upgradeTransfer=int(tmpList[self.POSMap['UpgradeTransfer']])
                except:
                    self.upgradeTransfer = "0"

            #get the factory reset flag
            if columnCnt >=self.POSMap['ResetTransfer']+1:
                try:
                    self.ResetTransfer=int(tmpList[self.POSMap['ResetTransfer']])
                except:
                    self.ResetTransfer = "0"

            #get the boot reset flag
            if columnCnt >=self.POSMap['BootAction']+1:
                try:
                    self.bootAction="\""+str(tmpList[self.POSMap['BootAction']]).strip()+"\""
                except:
                    self.bootAction = "\"\""

            #get the validityCheck
            if columnCnt >=self.POSMap['validityCheck']+1:
                try:
                    if str(tmpList[self.POSMap['validityCheck']]).strip() == "":
                        self.pValidityCheckHandler = "NULL"
                    else:
                        self.pValidityCheckHandler = str(tmpList[self.POSMap['validityCheck']])
                except:
                    self.pValidityCheckHandler = str("NULL")

            #get the UseOperatorDefault
            if columnCnt >=self.POSMap['UseOperatorDefault']+1:
                try:
                    self.UseOperatorDefault=int(tmpList[self.POSMap['UseOperatorDefault']])
                except:
                    self.UseOperatorDefault = "0"

            #get the UseHardwareDefault
            if columnCnt >=self.POSMap['UseHardwareDefault']+1:
                try:
                    self.UseHardwareDefault=int(tmpList[self.POSMap['UseHardwareDefault']])
                except:
                    self.UseHardwareDefault = "0"

            #get forceActiveNotification
            if columnCnt >=self.POSMap['forceActiveNotification']+1:
                try:
                    self.forceActiveNotification=int(tmpList[self.POSMap['forceActiveNotification']])
                except:
                    self.forceActiveNotification = "0"

            #get the Tr69Password
            if columnCnt >=self.POSMap['Tr69Password']+1:
                try:
                    self.Tr69Password=int(tmpList[self.POSMap['Tr69Password']])
                except:
                    self.Tr69Password = "0"

            #get the AsVendorConfig
            if columnCnt >=self.POSMap['AsVendorConfig']+1:
                try:
                    self.AsVendorConfig=int(tmpList[self.POSMap['AsVendorConfig']])
                except:
                    self.AsVendorConfig = "0"

            #get the AsKeyParam
            if columnCnt >=self.POSMap['AsKeyParam']+1:
                try:
                    self.AsKeyParam=int(tmpList[self.POSMap['AsKeyParam']])
                except:
                    self.AsKeyParam = "0"


            #get the bitmap for read/write access
            tmpCol=moduleItem.accessModuleStartCol
            for i in range(0, len(moduleItem.accessModuleList)):
                if re.search('R', str(tmpList[tmpCol+i]), re.IGNORECASE) :
                    self.readBitmap |= moduleItem.accessModuledict[moduleItem.accessModuleList[i]]
                if re.search('W', str(tmpList[tmpCol+i]), re.IGNORECASE) :
                    self.writeBitmap |= moduleItem.accessModuledict[moduleItem.accessModuleList[i]]


            #get the get-hooker handler
            tmpCol=moduleItem.getModuleStartCol
            for i in range(0, len(moduleItem.getModuleList)):
                #logger.debug("get hanlder i ="+str(i)+"/"+str(len(moduleItem.getModuleList)) +" StartCol "+str(tmpCol) + "mod " + moduleItem.getModuleList[i])
                self.getHandlerList[moduleItem.getModuleList[i]] = str(tmpList[tmpCol+i]).strip()

            #get the set-hooker handler
            tmpCol=moduleItem.setModuleStartCol
            for i in range(0, len(moduleItem.setModuleList)):
                #logger.debug("set hanlder i ="+str(i)+"/"+str(len(moduleItem.setModuleList)) +" StartCol "+str(tmpCol) + "mod " + moduleItem.setModuleList[i])
                self.setHandlerList[moduleItem.setModuleList[i]] = str(tmpList[tmpCol+i]).strip()

            #get the notify callback list
            #logger.debug('tmpList len = '+str(len(tmpList))+' notifyCBsStartCol = '+str(moduleItem.notifyCBsStartCol) + ' notifyCBsCnt = '+str(moduleItem.notifyCBsCnt))
            tmpCol = moduleItem.notifyModuleStartCol
            for i in range(0, len(moduleItem.notifyModuleList)):
                #logger.debug("notify hanlder i ="+str(i)+"/"+str(len(moduleItem.notifyModuleList)) +" StartCol"+str(tmpCol))
                self.notifyHandlerlist[moduleItem.notifyModuleList[i]] = str(tmpList[tmpCol+i]).strip()


        else:
            print("Error not enough information")

        self.siblingIndex=-1

        #process the name and set the next level
        self.processName(externConvertList, convertToString, convertToStruct)

        #print("leave parseLine")

class DataProcesser:
    """ the data process which will generate include, source, pattern file"""
    def __init__(self, excelBook, dbFileVersion):
        self.excelBook = excelBook
        self.dbFileVersion = dbFileVersion
        self.dbnames = []
        self.extDb = {}
        self.defaultValueMap = {}
        #self.gridData = gridData
        self.itemModuleList = {}
        self.rootName={}
        self.patternName={}
        self.itemCnt={}
        self.maxIndex={}
        self.itemList = {}
        self.itemXMLMap={}
        self.itemMap={}
        self.dataTypeMap={}
        self.cTypeMap={
                    "STRUCT":"C_TYPE_STRUCT"
                    }
        self.cTypeUsed={}

        self.convertToString={
                    "STRUCT":"NULL",
                    }
        self.convertToStruct={
                    "STRUCT":"NULL",
                    }
        self.validCheck={}
        self.externHeaderFiles={}
        self.hasExternType = 0

        self.externConvertList=[]
        #self.defaultValueDict={}

        self.attributeList=[]

        self.dbVersion = {}

        """"parse the default value sheet"""
##        for sheet in self.excelBook.sheets():
##            if sheet.name == "Defaule Value":
##                pass
                #create Default Value dict

        """filter the data and fillup the itemList variable"""
        for sheet in self.excelBook.sheets():
            if sheet.name.endswith("DefaultValue"):
                self.processDefaultValue(sheet)

            elif sheet.name == "CType Definition":
                self.processCType(sheet)

            elif sheet.name == "DataType Definition":
                self.processDataType(sheet)
            elif sheet.name == "XmlAttribute Definition":
                self.processXmlAttribute(sheet)

        for sheet in self.excelBook.sheets():
            if sheet.name.endswith(" Model") or sheet.name.endswith(" ExtModel"):
                cnt=1

                #get rootName
                dbname = sheet.cell_value(2,0).replace('.','').strip()
                logger.debug("dbname : "+str(dbname))

                self.dbnames.append(dbname)

                if sheet.name.endswith(" ExtModel"):
                    self.extDb[dbname] = 'DM_TRUE'
                else:
                    self.extDb[dbname] = 'DM_FALSE'

                self.rootName[dbname] =  sheet.cell_value(2,0)

                #create module list
                tmpModuleItem = ModuleItem()
                tmpModuleItem.parseModule(sheet.row_values(0),sheet.row_values(1))
                self.itemModuleList[dbname]=[]
                self.itemModuleList[dbname].append(tmpModuleItem)

                #create item list
                self.itemList[dbname]=[]
                self.itemMap[dbname]={}
                self.cTypeUsed[dbname]={}
                for i in range(2,sheet.nrows):
                    tmpItem = PatternItem()
                    tmpItem.parseLine(sheet.row_values(i),tmpModuleItem,self.externConvertList,self.convertToString,self.convertToStruct)
                    if(len(tmpItem.name.strip()) <= 0):
                        break
                    tmpItem.selfIndex=cnt-1
                    self.itemList[dbname].append(tmpItem)
                    cnt+=1

                    if tmpItem.name.strip() == 'VendorDataModelVersion':
                        if dbFileVersion != '':
                            try:
                                tmpItem.defaultValue = dbFileVersion
                                logger.debug(dbname+' VendorDataModelVersion value changed to '+tmpItem.defaultValue)
                            except:
                                pass
                        else:
                            pass

                        self.dbVersion[dbname]=tmpItem.defaultValue.strip()
                        logger.debug("final dbVersion : "+self.dbVersion[dbname])
                    else:
                        pass

                    self.cTypeUsed[dbname][str(tmpItem.cType)]=1

                    #set up the object map
                    if tmpItem.isObject:
                        self.itemMap[dbname][tmpItem.nodeFullName]=tmpItem

                self.maxIndex[dbname]=cnt-1
                logger.debug("maxIndex : "+str(self.maxIndex[dbname]))
            else:
                pass

        for dbname in self.dbnames:
            #set the parent index
            for tmpItem in self.itemList[dbname]:
                #the root item has no parent, so set it to -1
                if (tmpItem.nodeFullName == tmpItem.parentFullName):
                    tmpItem.parentIndex = -1
                elif tmpItem.parentFullName == "":                                  #error
                    logger.error("found error")
                else:
                    #logger.debug("child : "+str(tmpItem.orgName)+"  parent : "+str(tmpItem.parentFullName))
                    tmpItem.parentIndex = self.itemMap[dbname][tmpItem.parentFullName].selfIndex

            #set the first child index
            for tmpItem in self.itemList[dbname][0:-1]:
                n=tmpItem.selfIndex+1
                nextItem = self.itemList[dbname][n]
                while(tmpItem.selfIndex != nextItem.parentIndex):
                    n+=1
                    if(n >= len(self.itemList[dbname])):
                        break
                    nextItem = self.itemList[dbname][n]

                #found the first child
                if  tmpItem.selfIndex == nextItem.parentIndex:
                    tmpItem.childrenCounter = 0
                    tmpItem.firstChildIndex = nextItem.selfIndex
                    while(1):
                        n+=1
                        if tmpItem.selfIndex == nextItem.parentIndex:
                            tmpItem.childrenCounter += 1
                        else:
                            pass

                        if(n >= len(self.itemList[dbname])):
                            break
                        nextItem = self.itemList[dbname][n]
                #reach the end
                else:
                    tmpItem.childrenCounter = 0
                    tmpItem.firstChildIndex = -1

            #set the sibling index
            for tmpItem in self.itemList[dbname][0:-1]:
                n=tmpItem.selfIndex+1
                nextItem = self.itemList[dbname][n]
                while(tmpItem.parentIndex != nextItem.parentIndex):
                    n+=1
                    if(n >= len(self.itemList[dbname])):
                        break
                    nextItem = self.itemList[dbname][n]

                #found the sibling
                if  tmpItem.parentIndex == nextItem.parentIndex:
                    tmpItem.siblingIndex = nextItem.selfIndex
                #reach the end
                else:
                    tmpItem.siblingIndex = -1

            #handle self-defined prefix
            for tmpItem in self.itemList[dbname]:
                if self.checkIfAnyParentItemSelfDefined(tmpItem, dbname) == False:
                    #none of its parents are not self-defined
                    if tmpItem.selfdefined == "Y":
                        if tmpItem.isObject == False:
                            #parameter
                            tmpItem.orgNameReplace = SelfDefinedPrefix+tmpItem.orgName
                            tmpItem.nodeFullNameReplace = self.itemList[dbname][tmpItem.parentIndex].nodeFullNameReplace+SelfDefinedPrefix+tmpItem.name
                            tmpItem.nameReplace = SelfDefinedPrefix+tmpItem.name
                        else:
                            #object
                            if tmpItem.isMultObj == False:
                                tmpItem.orgNameReplace = self.itemList[dbname][tmpItem.parentIndex].orgNameReplace+SelfDefinedPrefix+tmpItem.name+"."
                                tmpItem.nodeFullNameReplace = self.itemList[dbname][tmpItem.parentIndex].nodeFullNameReplace+SelfDefinedPrefix+tmpItem.name+"."
                                tmpItem.nameReplace = SelfDefinedPrefix+tmpItem.name
                            else:
                                tmpItem.orgNameReplace = self.itemList[dbname][tmpItem.parentIndex].orgNameReplace+SelfDefinedPrefix+tmpItem.name+".{i}."
                                tmpItem.nodeFullNameReplace = self.itemList[dbname][tmpItem.parentIndex].nodeFullNameReplace+SelfDefinedPrefix+tmpItem.name+"."
                                tmpItem.nameReplace = SelfDefinedPrefix+tmpItem.name
                    else:
                        #non-self-defined
                        tmpItem.orgNameReplace = tmpItem.orgName
                        tmpItem.nodeFullNameReplace = tmpItem.nodeFullName
                        tmpItem.nameReplace = tmpItem.name
                else:
                    #its parent is self-defined
                    if tmpItem.isObject == False:
                        #parameter
                        tmpItem.orgNameReplace = tmpItem.orgName
                        tmpItem.nodeFullNameReplace = self.itemList[dbname][tmpItem.parentIndex].nodeFullNameReplace+tmpItem.name
                        tmpItem.nameReplace = tmpItem.name
                    else:
                        #object
                        if tmpItem.isMultObj == False:
                            tmpItem.orgNameReplace = self.itemList[dbname][tmpItem.parentIndex].orgNameReplace+tmpItem.name+"."
                            tmpItem.nodeFullNameReplace = self.itemList[dbname][tmpItem.parentIndex].nodeFullNameReplace+tmpItem.name+"."
                            tmpItem.nameReplace = tmpItem.name
                        else:
                            tmpItem.orgNameReplace = self.itemList[dbname][tmpItem.parentIndex].orgNameReplace+tmpItem.name+".{i}."
                            tmpItem.nodeFullNameReplace = self.itemList[dbname][tmpItem.parentIndex].nodeFullNameReplace+tmpItem.name+"."
                            tmpItem.nameReplace = tmpItem.name


            #handle default-value-table
            for tmpItem in self.itemList[dbname]:
                if tmpItem.defaultValue.startswith("#Ref:"):
                    refprefix,refsheet,refrow = tmpItem.defaultValue.split(":")
                    #logger.debug("handle "+tmpItem.nodeFullNameReplace+" default-values : prefix="+str(refprefix)+" sheet="+str(refsheet)+" refrow="+str(refrow))

                    self.processDefaultValueTable(tmpItem, refsheet, refrow)


    def checkIfAnyParentItemSelfDefined(self, tmpItem, dbname):
        isSelfDefined = False

        parentItem = self.itemList[dbname][tmpItem.parentIndex]

        while True:
            if parentItem.selfdefined != "Y":
                if parentItem.parentIndex >= 0:
                    parentItem = self.itemList[dbname][parentItem.parentIndex]
                else:
                    break
            else:
                isSelfDefined = True
                break

        return isSelfDefined


###begin of Include File

    def processDefaultValueTable(self, tmpItem, refsheet, refrow):
        top_row = int(str(refrow)[3:])-1
        instanceNum_column = 0

        strippedItemPath = str(tmpItem.nodeFullNameReplace).strip().replace("{i}","")

        for sheet in self.excelBook.sheets():
            if sheet.name.strip() == str(refsheet).strip():
                for celltext in sheet.row_values(top_row):
                    if str(celltext).strip() == "InstanceNumber":
                        break
                    instanceNum_column+=1

                #find the row & column of instance-number cell
                #logger.debug("top_row "+str(top_row)+" instanceNum_column "+str(instanceNum_column))

                for i in range(instanceNum_column+1,sheet.ncols):
                    #find the item column
                    if str(sheet.cell_value(top_row,i)).strip() == str(tmpItem.orgName).strip():
                        for j in range(top_row+1,sheet.nrows):
                            instanceNumList = str(sheet.cell_value(j,instanceNum_column)).strip()

                            if re.search("\.0$", str(instanceNumList)):
                                instanceNumList = str(instanceNumList)[:-2]

                            strippedItemValue = str(sheet.cell_value(j,i)).strip().replace('"','\\"').replace('\r','').replace('\n','\\n')

                            if re.search("\.0$", str(strippedItemValue)):
                                strippedItemValue = str(strippedItemValue)[:-2]

                            if len(instanceNumList)>0:
                                if self.defaultValueMap.has_key(strippedItemPath):
                                    self.defaultValueMap[strippedItemPath] = self.defaultValueMap[strippedItemPath] + '[' + instanceNumList + ']' + "="+strippedItemValue
                                else:
                                    self.defaultValueMap[strippedItemPath] = '[' + instanceNumList + ']' + "="+ strippedItemValue
                            else:
                                break
                        #logger.debug("default-value list : "+str(self.defaultValueMap[strippedItemPath]))
                        break


    def processCType(self, sheet):
        for i in range(2,sheet.nrows):
            tmp,ctype,description,defheader,validcheck,tostring,tostruct=sheet.row_values(i)

            if len(str(ctype).strip()) > 0:
                ctypeTmp = str(ctype).strip().upper().replace(' ', '_')

                if ctypeTmp[-2:] == '[]':
                    self.cTypeMap[str(ctype).strip()] = 'C_TYPE_'+ctypeTmp[:-2]+'_ARRAY'
                else:
                    self.cTypeMap[str(ctype).strip()] = 'C_TYPE_'+ctypeTmp

                self.validCheck[str(ctype).strip()] = str(validcheck).strip()
                self.convertToString[str(ctype).strip()] = str(tostring).strip()
                self.convertToStruct[str(ctype).strip()] = str(tostruct).strip()

                if len(str(defheader).strip()) > 0 and str(defheader).strip() != "build-in":
                    self.externHeaderFiles[str(ctype).strip()] = str(defheader).strip()

    def processDataType(self, sheet):
        for i in range(2,sheet.nrows):
            tmp,datatype,description=sheet.row_values(i)

            if len(str(datatype).strip()) > 0:
                datatypeTmp = str(datatype).strip().upper().replace(' ', '_')
                self.dataTypeMap[str(datatype).strip()] = 'DATA_TYPE_'+datatypeTmp

    def processXmlAttribute(self, sheet):
        for i in range(2,sheet.nrows):
            tmp,name,description,createdbydefault,ctype,valuerange,defaultvalue=sheet.row_values(i)

            if len(str(name).strip()) > 0:
                tmpList=[]
                tmpList.append(str(name).strip())
                tmpList.append(str(description).strip().replace('\n','\\n').replace('\r',''))
                if str(createdbydefault).strip().endswith('.0'):
                    tmpList.append(str(createdbydefault).strip()[:-2])
                else:
                    tmpList.append(str(createdbydefault).strip())
                tmpList.append(self.cTypeMap[str(ctype).strip()])
                tmpList.append(str(ctype).strip())
                if str(valuerange).strip().endswith('.0'):
                    tmpList.append(str(valuerange).strip().replace('\n',',')[:-2])
                else:
                    tmpList.append(str(valuerange).strip().replace('\n',','))
                if str(defaultvalue).strip().endswith('.0'):
                    tmpList.append(str(defaultvalue).strip()[:-2])
                else:
                    tmpList.append(str(defaultvalue).strip())
                tmpList.append(self.validCheck[str(ctype).strip()])
                tmpList.append(self.convertToString[str(ctype).strip()])
                tmpList.append(self.convertToStruct[str(ctype).strip()])

                self.attributeList.append(tmpList)

    def processDefaultValueName(self, name):
        nameA = name.split(".")
        name = ""
        arrayIndex = []
        for nameT in nameA:
            try:
                i = int(nameT)
                arrayIndex.append(i)
            except:
                if len(name) > 0:
                    name = name + "." + nameT
                else:
                    name = nameT
        return  name, str(arrayIndex).replace(" ", "")

    def processDefaultValue(self, sheet):
        self.defaultValueMap = {}
        for i in range(1,sheet.nrows):
            #logger.debug(sheet.row_values(i))
            name,value=sheet.row_values(i)
            if re.search("\.0$", str(value)):
                value = str(value)[:-2]
            else:
                value = str(value)

            nameKey, arrayIdx = self.processDefaultValueName(name)
            if self.defaultValueMap.has_key(nameKey):
                self.defaultValueMap[nameKey] = self.defaultValueMap[nameKey] + arrayIdx + "="+value
            else:
                self.defaultValueMap[nameKey] = arrayIdx + "="+ str(value)
#        for tmpK in self.defaultValueMap.keys():
#            logger.debug(tmpK)
#            logger.debug(self.defaultValueMap[tmpK])

    def convertRangeToInt(self, value):
        if str(value).find('[') == -1:
            return int(float(str(value)))
        else:
            return int(float(str(value)[:str(value).find('[')]))

    def formatIDDef(self, list):
        """
        the each item of list should be [a,b],
        a is the "#define XXX"
        b is the "0xxx"
        """
        maxLen=-1
        for tmpItem in list:
            if maxLen < len(tmpItem[0]):
                maxLen = len(tmpItem[0])

        return maxLen

    def getIDName(self, item):
        if item.isObject:
            return item.nodeFullName.replace(".", "_").upper()
        else:
            return item.nodeFullName.replace(".", "_").upper()+"_"

    def genDefineID(self, f, dbname):
        """
        generate the id definition, and write it to the file
        """
        #generate the ID definition
        tmpIDDefList=[]
        for tmpItem in self.itemList[dbname]:
            if tmpItem.isObject:
                tmpDefinition = ["#define " + tmpItem.definition, " 0x{0:x} ".format(tmpItem.selfIndex), "/* "+ tmpItem.orgNameReplace + " */\n"]
            else:
                tmpDefinition = ["#define " + tmpItem.definition, " 0x{0:x} ".format(tmpItem.selfIndex), "/* "+ self.itemList[dbname][tmpItem.parentIndex].orgNameReplace + tmpItem.orgNameReplace + " */\n"]
            tmpIDDefList.append(tmpDefinition)

        #get the max len of #define
        maxLen=-1
        for tmpItem in tmpIDDefList:
            if maxLen < len(tmpItem[0]):
                maxLen = len(tmpItem[0])
        #write the define to the profile
        for tmpItem in tmpIDDefList:
            f.write(tmpItem[0].ljust(maxLen)+tmpItem[1].ljust(10)+tmpItem[2])
        return

    def genObjectStructDef(self, strList, item):
        tmpStr = "/* #define {0}ID 0x{1:x} */\n".format( self.getIDName(item), item.selfIndex)
        strList.append(tmpStr)

        valueRange = item.valueRange
        if item.isMultObj:
            if valueRange != "" and valueRange !="*" and valueRange !="0":
                tmpStr = "#define {0}CNT 0x{1:x}\n".format( self.getIDName(item), self.convertRangeToInt(item.valueRange))
                tmpMacroName = "{0}CNT".format(self.getIDName(item))
                strList.append(tmpStr)
                strList.append([" "*4 + "UINT32",  " "*4 + item.name + "_cnt;\n"])
                strList.append([" "*4+ self.getIDName(item)+"T", " "*4 + item.name + "[{0}];\n\n".format(tmpMacroName)])
            elif valueRange == "*":
                tmpStr = "#define {0}CNT 0x{1:x}\n".format(self.getIDName(item), -1)
                tmpMacroName = "{0}CNT".format(self.getIDName(item))
                strList.append(tmpStr)
                strList.append([" "*4 + "UINT32", " "*4 + item.name + "_cnt;\n"])
                strList.append([" "*4+ self.getIDName(item)+ "T",  " "*4 + item.name + "[{0}];\n\n".format(tmpMacroName)])
            else:
                strList.append([" "*4+ self.getIDName(item)+ "T",  " "*4 + item.name + ";\n\n"])
        else:
            if valueRange !="*" :
                strList.append([" "*4+ self.getIDName(item)+ "T", " "*4 + item.name + ";\n\n"])
            else :
                strList.append([" "*4 + "UINT32", " "*4 + item.name + "_cnt;\n"])
                strList.append([" "*4+ self.getIDName(item)+ "T", " "*4 + item.name + ";\n\n"])

    def genArrayStructDef(self, strList, item):
        tmpStr = "/* #define {0}ID 0x{1:x} */\n".format( self.getIDName(item), item.selfIndex)
        strList.append(tmpStr)
        if item.cType == "INT8[]":
            tmpStr = "#define {0}CNT 0x{1:x}\n".format( self.getIDName(item), self.convertRangeToInt(item.valueRange)+1)
        else:
            tmpStr = "#define {0}CNT 0x{1:x}\n".format( self.getIDName(item), self.convertRangeToInt(item.valueRange))
        strList.append(tmpStr)
        tmpMacroName = "{0}CNT".format(self.getIDName(item))

        strList.append([" "*4+"UINT32" ,  " "*4+ item.name + "Cnt;\n"])
        strList.append([" "*4+item.cType.strip()[0:-2] ,  " "*4+ item.name + "["+tmpMacroName+"];\n\n"])

    def genBitmapStructDef(self, strList, item):
        #write the id definition of the member
        tmpStr = "/* #define {0}ID 0x{1:x} */\n".format( self.getIDName(item), item.selfIndex)
        strList.append(tmpStr)

        #process the value range to the macro definition
        tmpDefList = [];
        tmpVarRangeArray = item.valueRange.split("\n")
        for  i in range(0, len(tmpVarRangeArray)):
            defStr = "#define {0}{1} ".format( self.getIDName(item), \
                                tmpVarRangeArray[i].replace(" ","_").replace("-","_").replace("*","X").upper())
            valueStr = "0x{0:0>8X} \n".format(1<<i)
            tmpDefList.append([defStr, valueStr])

        tmpMaxIdLen = self.formatIDDef(tmpDefList)

        for tmpDefItem in tmpDefList:
            strList.append(tmpDefItem[0].ljust(tmpMaxIdLen)+tmpDefItem[1])

        strList.append([" "*4+"UINT32" ,  " "*4+ item.name + ";\n\n"])

    def genItemStructDef(self, strList, item):
        tmpStr = "/* #define {0}ID 0x{1:x} */\n".format( self.getIDName(item), item.selfIndex)
        strList.append(tmpStr)
        strList.append([" "*4+item.cType ,  " "*4+ item.name + ";\n\n"])

    def genEnumStructDef(self, strList, item):
        tmpStr = "/* #define {0}ID 0x{1:x} */\n".format( self.getIDName(item), item.selfIndex)
        strList.append(tmpStr)

        #process the value range to the macro definition
        tmpDefList = [];
        tmpVarRangeArray = item.valueRange.split("\n")
        for  i in range(0, len(tmpVarRangeArray)):
            defStr = "#define {0}{1} ".format( self.getIDName(item), tmpVarRangeArray[i].\
                        replace(" ","_").replace("-","_").replace("/","_").replace("*","X").upper())
            valueStr = "0x{0:X} \n".format(i)
            tmpDefList.append([defStr, valueStr])

        tmpMaxIdLen = self.formatIDDef(tmpDefList)

        for tmpDefItem in tmpDefList:
            strList.append(tmpDefItem[0].ljust(tmpMaxIdLen)+tmpDefItem[1])

        strList.append([" "*4+"UINT32" ,  " "*4+ item.name + ";\n\n"])

    def genStructDef(self, f, tmpItem,level, type, dbname):
        if tmpItem.isObject and tmpItem.objLevel == level:

            strList=[]

            #generat the struct
            tmpStr = 'typedef struct ' + self.getIDName(tmpItem).lower() + "t" + '{\n'
            strList.append(tmpStr)

            #if multiple object, generate the instance number
            if tmpItem.isMultObj:
                strList.append([" "*4+"UINT32" ,  " "*4+ "instanceNum;\n\n"])

            #generate the member of struct
            for tmpChildItem in self.itemList[dbname]:
                if tmpChildItem.parentIndex == tmpItem.selfIndex:
                    #process object member
                    if tmpChildItem.isObject:
                        self.genObjectStructDef(strList, tmpChildItem)
                    #process array
                    elif tmpChildItem.isSimpleArray:
                        self.genArrayStructDef(strList, tmpChildItem)
                    else:
                        #the type is customized, and the AAA_BBB AAA is the type to be used in C type, BBB is to indicate how to process the value
                        if tmpChildItem.cType == "UINT32_BITMAP":
                            self.genBitmapStructDef(strList, tmpChildItem)
                        elif tmpChildItem.cType == "UINT32_ENUM":
                            self.genEnumStructDef(strList, tmpChildItem)
                        else:
                            self.genItemStructDef(strList, tmpChildItem)

            #generate the end of struct definition
            tmpStr = '}'+ tmpItem.nodeFullName.replace(".","_").upper()+ "T"+';\n\n\n'
            strList.append(tmpStr)

            #calculate the max length of first column
            maxLen = 0
            for tmpStr in strList:
                if isinstance(tmpStr,  list):
                   if maxLen < len(tmpStr[0]) :
                       maxLen = len(tmpStr[0])

            #write to file
            for tmpStr in strList:
                if isinstance(tmpStr,  list):
                    f.write(tmpStr[0].ljust(maxLen)+tmpStr[1])
                else:
                    f.write(tmpStr)
        return

    def genIncludeFile(self, filePath, dbname, fileName):

        currentLevel = self.itemList[dbname][0].__class__.maxLevel

        #open the include file
        if not os.path.exists(ResultSrcDir + "/db_" + str(dbname).lower() + "/inc/"):
            os.makedirs(ResultSrcDir + "/db_" + str(dbname).lower() + "/inc/")

        logger.debug('writing '+ResultSrcDir + "/db_" + str(dbname).lower() + "/inc/" + fileName)
        f=open(ResultSrcDir + "/db_" + str(dbname).lower() + "/inc/" + fileName, "wb")

        #write the file header
        hFileName = "_" + fileName.replace(".", "_").upper() + "_"
        f.write("#ifndef " + hFileName + "\n")
        f.write("#define " + hFileName + "\n")
        f.write("\n")
        f.write("#ifdef __cplusplus\n")
        f.write("extern \"C\" {\n")
        f.write("#endif\n")
        f.write("\n")

        f.write("\n")
        f.write('#include "common_type_def.h"\n\n')

        f.write("/* the declaration of external header files if any used */\n")
        self.genExtHeaders(dbname,f)

        f.write("\n"*2)

        #write the data type definition
        f.write("/* the definition of data type */\n")
        self.genDataTypeDef(dbname, f)

        f.write("\n"*2)

        #write the c type definition
        f.write("/* the definition of c type */\n")
        self.genCTypeDef(dbname, f)

        #write extra c-type definition
        f.write("\n"*2)
        f.write('#ifndef _INT8A\n')
        f.write('#define _INT8A\n')
        f.write('typedef INT8 INT8A;\n')
        f.write('#endif\n')
        f.write("\n"*2)
        f.write('#ifndef _UINT32_HEX\n')
        f.write('#define _UINT32_HEX\n')
        f.write('typedef UINT32 UINT32_HEX;\n')
        f.write('#endif\n')
        f.write("\n"*2)
        f.write('#ifndef _UINT32_ENUM\n')
        f.write('#define _UINT32_ENUM\n')
        f.write('typedef UINT32 UINT32_ENUM;\n')
        f.write('#endif\n')
        f.write("\n"*2)
        f.write('#ifndef _UINT32_BITMAP\n')
        f.write('#define _UINT32_BITMAP\n')
        f.write('typedef UINT32 UINT32_BITMAP;\n')
        f.write('#endif\n')
        f.write("\n"*2)

        #write the items definition

        f.write("\n"*2)
        f.write("/* the definition of parameter id */\n")
        self.genDefineID(f, dbname)
        f.write("\n"*2)

        #generate the profile struct definition

        f.write("/* the definition of c structure */\n")
        while (currentLevel > 0):
            for tmpItem in self.itemList[dbname]:
                #generate the struct definition
                self.genStructDef(f, tmpItem, currentLevel,type,dbname)
            currentLevel -= 1

        #generate loading-functions.
        f.write('\n'*2)
        f.write('/* the declaration of '+dbname+' model loading function */\n')
        f.write('extern INT32 '+dbname.capitalize()+'DbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath);\n')

        f.write('\n'*2)

        f.write("#ifdef __cplusplus\n")
        f.write("}\n")
        f.write("#endif\n")
        f.write("\n")
        f.write("#endif\n")

        f.close()
        return

###end of Include File

    def genDataValidCheckDeclaration(self, f, dbname):
        DataValidCheckUsed = {}

        for cTypeUsed in self.cTypeUsed[dbname].keys():
            try:
                #logger.debug(cTypeUsed+" validCheck "+self.validCheck[cTypeUsed])
                DataValidCheckUsed[str(self.validCheck[cTypeUsed])]=1
            except:
                continue

        for key in sorted(DataValidCheckUsed.keys(), key=str.lower):
            try:
                if key != '':
                    f.write('extern BOOL8 '+key+'(INT8 *pValueRange, INT8 *pValue);\n')
            except:
                continue

    def genDataTypeDef(self, dbname, f):
        maxLen=-1
        for key in self.dataTypeMap.keys():
            if maxLen < len(str(dbname).upper()+'_'+self.dataTypeMap[key]):
                maxLen = len(str(dbname).upper()+'_'+self.dataTypeMap[key])

        i=0
        for key in sorted(self.dataTypeMap.keys(), key=str.lower):
            f.write("#define "+str(dbname).upper()+'_'+self.dataTypeMap[key].ljust(maxLen)+" "*8+str(hex(i)).ljust(10)+" "*4+"/* "+key+" */"+"\n")
            i += 1

    def genCTypeDef(self, dbname, f):
        maxLen=-1
        for key in self.cTypeMap.keys():
            if maxLen < len(str(dbname).upper()+'_'+self.cTypeMap[key]):
                maxLen = len(str(dbname).upper()+'_'+self.cTypeMap[key])

        i=0
        for key in sorted(self.cTypeMap.keys(), key=str.lower):
            f.write("#define "+str(dbname).upper()+'_'+self.cTypeMap[key].ljust(maxLen)+" "*8+str(hex(i)).ljust(10)+" "*4+"/* "+key+" */"+"\n")
            i += 1

    def genExtHeaders(self, dbname, f):
        externhdrused = {}

        for cTypeUsed in self.cTypeUsed[dbname].keys():
            try:
                cTypeKey=self.externHeaderFiles[cTypeUsed]
                externhdrused[cTypeKey]=1
            except:
                continue

        for key in externhdrused.keys():
            if key.strip() != "build-in":
                self.hasExternType = 1
                f.write("#include \""+str(dbname).lower()+"_"+key+".h\"\n")


###begin of Pattern File

    def calculateMaxColumn(self, structList,  columnLenList):
        """
        #calculate the len of each line
        """
        i=0
        while i < len(structList):
            if len(columnLenList) <= i:
                columnLenList.append(len(structList[i]))
            else:
                if columnLenList[i] < len(structList[i]):
                    columnLenList[i] = len(structList[i])
            i +=1
    def cPtnDftValueGen(self, patternItem):
        if self.defaultValueMap.has_key(patternItem.nodeFullNameReplace):
            return '\"' + self.defaultValueMap[patternItem.nodeFullNameReplace] + '\"'
        defaultValue = patternItem.defaultValue
        defaultValue = '"'+str(defaultValue)+'"'
        if len(defaultValue) > 0 and (defaultValue[0] != '"' or defaultValue[1] != '['):
            return defaultValue
        #get the array depth, [ = 1, [[ = 2, [[[ = 3
        arrayDepth = 0
        indexArray = []
        iCnt = 0
        retValue = ""
        while((defaultValue[iCnt] == '[' or defaultValue[iCnt] == '\"') and iCnt < len(defaultValue)-1):
            if(defaultValue[iCnt] == '[') :
                indexArray.append(0)
                arrayDepth = arrayDepth + 1
            iCnt = iCnt + 1
        #get the each default value, and up the array depth, ]=>0,1 ]]=>0,0,1, ]]]=>0,0,0,1
        retValue = ""
        curDftValue = ""
        while(iCnt < len(defaultValue) -1):
            if(defaultValue[iCnt] == '['):
                arrayDepth = arrayDepth + 1
            if(defaultValue[iCnt] == ']'):
                arrayDepth = arrayDepth -1
            if(defaultValue[iCnt] == ',' or (defaultValue[iCnt] == ']' and defaultValue[iCnt-1] != ']')):
                #retValue = retValue + str(indexArray).replace(" ", "") + "=" + curDftValue
                tmpA = []
                for tmpI in indexArray:
                    tmpA.append(tmpI+1)
                retValue = retValue + str(tmpA).replace(" ", "") + "=" + curDftValue
                curDftValue = ""
            if (defaultValue[iCnt] == ']' or defaultValue[iCnt] == ',') and arrayDepth > 0:
                indexArray[arrayDepth-1] = indexArray[arrayDepth-1] + 1
            if(defaultValue[iCnt] != '[' and defaultValue[iCnt] != ']' and defaultValue[iCnt] != ','):
                curDftValue = curDftValue + defaultValue[iCnt]
            if defaultValue[iCnt] == '"':
                iCnt = iCnt + 1
                while defaultValue[iCnt] != '"':
                    curDftValue = curDftValue + defaultValue[iCnt]
                    iCnt = iCnt + 1
                curDftValue = curDftValue + defaultValue[iCnt]
            iCnt = iCnt + 1
        #return the default value
        return '\"' + retValue + '\"'
    def cPatternLineGen(self, dbname, patternItem):
        """
        generate the one line c struct
        Name,  Description,  ParaType,  CType,   PIdx,  SIdx,  R(bitmap), W(bitmap),   Range,    Dft,  NodeHandler,  NotifyHandler, GetHandler  , NeedRest,  Tr69Password, pToStringFunc,  pToStructFunc
        """
        lineList = [];

        #generate nodeFullName of object or itemName
        if patternItem.isSimpleArray:
            tmpColumn = '"'+patternItem.nameReplace+'"'
        else:
            tmpColumn = '"'+patternItem.orgNameReplace+'"'
        lineList.append(" "*8+"/* Name */\n"+" "*8+"(INT8*)"+tmpColumn+",\n\n")

        #generate realname of object or itemName
        if patternItem.isSimpleArray:
            realName = patternItem.nameReplace
        else:
            realName = patternItem.orgNameReplace
        #logger.debug('orignal '+realName)
        if realName.endswith('.{i}.'):
            realName = realName[:-5]
            #logger.debug('remove .{i}. '+realName)
        elif realName.endswith('.'):
            realName = realName[:-1]
            #logger.debug('remove . '+realName)
        else:
            pass
        if not realName.rfind('.') == -1:
            realName = realName[realName.rfind('.')+1:]
            #logger.debug('remove prefix '+realName)
        else:
            pass
        tmpColumn = '"'+realName+'"'
        lineList.append(" "*8+"/* RealName */\n"+" "*8+"(INT8*)"+tmpColumn+",\n\n")

        #generate description
        tmpColumn = '"'+patternItem.description+'"'
        lineList.append(" "*8+"/* Description */\n"+" "*8+"(INT8*)"+tmpColumn+",\n\n")

        #generate data type
        #logger.debug("patternItem.type : "+str(patternItem.type))
        if patternItem.type in self.dataTypeMap:
            tmpColumn = str(dbname).upper()+'_'+self.dataTypeMap[patternItem.type]
        else:
            tmpColumn = '"unknown"'
        lineList.append(" "*8+"/* ParaType */\n"+" "*8+tmpColumn+",\n\n")

        #generate c type
        if patternItem.cType in self.cTypeMap:
            tmpColumn = str(dbname).upper()+'_'+self.cTypeMap[patternItem.cType]
        else:
            tmpColumn = '"unknown"'
        lineList.append(" "*8+"/* CType */\n"+" "*8+tmpColumn+",\n\n")

        #generate c type offset
        if patternItem.parentIndex>=0:
            parentItem=self.itemList[dbname][patternItem.parentIndex]
            tmpColumn = 'DM_OFFSETOF('+parentItem.nodeFullName.replace(".","_").upper()+ 'T,'+realName.replace(SelfDefinedPrefix,'')+')'
        else:
            tmpColumn = '0'
        lineList.append(" "*8+"/* CType Offset */\n"+" "*8+tmpColumn+",\n\n")

        #generate c-type-size
        if patternItem.cType == "STRUCT":
            tmpColumn = 'sizeof('+patternItem.nodeFullName.replace(".","_").upper()+ 'T)'
        elif patternItem.isSimpleArray:
            tmpColumn = 'sizeof('+patternItem.cType.strip()[0:-2]+')'
        else:
            tmpColumn = 'sizeof('+patternItem.cType+')'
        lineList.append(" "*8+"/* CTypeSize */\n"+" "*8+tmpColumn+",\n\n")

        #generate max-elements
        if patternItem.cType == "STRUCT":
            if patternItem.isMultObj:
                tmpColumn = "{0}CNT".format(self.getIDName(patternItem))
            else:
                tmpColumn = '0'
        elif patternItem.isSimpleArray:
            tmpColumn = "{0}CNT".format(self.getIDName(patternItem))
        else:
            tmpColumn = '0'
        lineList.append(" "*8+"/* MaxElements */\n"+" "*8+tmpColumn+",\n\n")

        #generate element cnt offset
        if patternItem.cType == "STRUCT":
            if patternItem.isMultObj:
                tmpColumn = 'DM_OFFSETOF('+parentItem.nodeFullName.replace(".","_").upper()+ 'T,'+realName.replace(SelfDefinedPrefix,'')+'_cnt)'
            else:
                tmpColumn = '0'
        elif patternItem.isSimpleArray:
            tmpColumn = 'DM_OFFSETOF('+parentItem.nodeFullName.replace(".","_").upper()+ 'T,'+realName.replace(SelfDefinedPrefix,'')+'Cnt)'
        else:
            tmpColumn = '0'
        lineList.append(" "*8+"/* ElementCntOffset */\n"+" "*8+tmpColumn+",\n\n")

        #generate parent index
        tmpColumn =str(patternItem.parentIndex)
        lineList.append(" "*8+"/* ParentIndex */\n"+" "*8+tmpColumn+",\n\n")

        #generate sibling index
        tmpColumn =str(patternItem.siblingIndex)
        lineList.append(" "*8+"/* SiblingIndex */\n"+" "*8+tmpColumn+",\n\n")

        #generate first child index
        tmpColumn =str(patternItem.firstChildIndex)
        lineList.append(" "*8+"/* FirstChildIndex */\n"+" "*8+tmpColumn+",\n\n")

        #generate children counter
        tmpColumn =str(patternItem.childrenCounter)
        lineList.append(" "*8+"/* ChildrenCounter */\n"+" "*8+tmpColumn+",\n\n")

        #generate read flag
        tmpColumn = str(hex(patternItem.readBitmap))
        lineList.append(" "*8+"/* ReadFlag */\n"+" "*8+tmpColumn+",\n\n")

        #generate write flag
        tmpColumn = str(hex(patternItem.writeBitmap))
        lineList.append(" "*8+"/* WriteFlag */\n"+" "*8+tmpColumn+",\n\n")

        #generate range of value
        tmpColumn ='"'+str(patternItem.valueRange).replace("\n", ",")+'"'
        lineList.append(" "*8+"/* ValueRange */\n"+" "*8+"(INT8*)"+tmpColumn+",\n\n")

        #generate default value
        if patternItem.defaultValue == "-":
            tmpColumn = '""'
        else:
            tmpColumn = self.cPtnDftValueGen(patternItem)

        lineList.append(" "*8+"/* DefaultValue */\n"+" "*8+"(INT8*)"+tmpColumn+",\n\n")

        #generate supportedXmlAttrs
        tmpColumn =str(patternItem.supportedXmlAttrs)
        lineList.append(" "*8+"/* supportedXmlAttrs */\n"+" "*8+"(INT8*)"+tmpColumn+",\n\n")

        #generate defaultXmlAttrValue
        tmpColumn =str(patternItem.defaultXmlAttrValue)
        lineList.append(" "*8+"/* defaultXmlAttrValue */\n"+" "*8+"(INT8*)"+tmpColumn+",\n\n")

        #generate XmlAttrNotifyEntities
        tmpColumn =str(patternItem.XmlAttrNotifyEntities)
        lineList.append(" "*8+"/* XmlAttrNotifyEntities */\n"+" "*8+"(INT8*)"+tmpColumn+",\n\n")

        #generate nodehandler list
        tmpColumn ="NULL"
        lineList.append(" "*8+"/* NodeHandler */\n"+" "*8+tmpColumn+",\n\n")

        #generate notifyhandler list
        tmpColumn ="NULL"
        lineList.append(" "*8+"/* NotifyHandler */\n"+" "*8+tmpColumn+",\n\n")

        #generate gethandler list
        tmpColumn ="NULL"
        lineList.append(" "*8+"/* GetHandler */\n"+" "*8+tmpColumn+",\n\n")

        #generate sethandler list
        tmpColumn ="NULL"
        lineList.append(" "*8+"/* SetHandler */\n"+" "*8+tmpColumn+",\n\n")

        #generate auto saved flag
        tmpColumn = str(patternItem.autoSaved)
        lineList.append(" "*8+"/* AutoSaved */\n"+" "*8+tmpColumn+",\n\n")

        #generate upgrade transfer flag
        tmpColumn = str(patternItem.upgradeTransfer)
        lineList.append(" "*8+"/* UpgradeTransfer */\n"+" "*8+tmpColumn+",\n\n")

        #generate factory reset flag
        tmpColumn = str(patternItem.ResetTransfer)
        lineList.append(" "*8+"/* ResetTransfer */\n"+" "*8+tmpColumn+",\n\n")

        #generate boot action
        tmpColumn = str(patternItem.bootAction)
        lineList.append(" "*8+"/* BootAction */\n"+" "*8+tmpColumn+",\n\n")

        #generate pValidityCheckHandler
        tmpColumn = str(patternItem.pValidityCheckHandler)
        lineList.append(" "*8+"/* ValidityCheckHandler */\n"+" "*8+tmpColumn+",\n\n")

        #generate UseOperatorDefault
        tmpColumn = str(patternItem.UseOperatorDefault)
        lineList.append(" "*8+"/* OperatorDefault */\n"+" "*8+tmpColumn+",\n\n")

        #generate UseHardwareDefault
        tmpColumn = str(patternItem.UseHardwareDefault)
        lineList.append(" "*8+"/* UseHardwareDefault */\n"+" "*8+tmpColumn+",\n\n")

        #generate forceActiveNotification
        tmpColumn = str(patternItem.forceActiveNotification)
        lineList.append(" "*8+"/* forceActiveNotification */\n"+" "*8+tmpColumn+",\n\n")

        #generate Tr69Password
        tmpColumn = str(patternItem.Tr69Password)
        lineList.append(" "*8+"/* Tr69Password */\n"+" "*8+tmpColumn+",\n\n")

        #generate pToStringFunc
        tmpColumn =str(patternItem.pToStringFunc)
        lineList.append(" "*8+"/* ToStringFunc */\n"+" "*8+tmpColumn+",\n\n")

        #generate pToStructFunc
        tmpColumn =str(patternItem.pToStructFunc)
        lineList.append(" "*8+"/* ToStructFunc */\n"+" "*8+tmpColumn)

        #generate the end of struct
        return lineList

    def genPatternFile(self, filePath, dbname, fileName, modelhdr):
        """
        generate the Pattern file
        """
        contList=[]
        #maxColumnList=[]


        #generate each line of pattern and calculate the max length of each column
        self.itemCnt[dbname]=0
        for tmpItem in self.itemList[dbname]:
            contList.append(self.cPatternLineGen(dbname, tmpItem))
        #    self.calculateMaxColumn(contList[-1],  maxColumnList)
            self.itemCnt[dbname] = self.itemCnt[dbname]+1

        if not os.path.exists(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/"):
            os.makedirs(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/")

        logger.debug('writing '+ResultSrcDir + "/db_" + str(dbname).lower() + "/src/" + fileName)
        f=open(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/" + fileName,  'wb')

        #write the include
        f.write('\n'*2)
        f.write("#include \"dm_internal.h\"\n")
        f.write('#include "'+modelhdr+'"\n\n')

        f.write('#ifdef DMALLOC_CHECK_ENABLED\n')
        f.write('#warning ***** compilation with dmalloc *****\n')
        f.write('#include "dmalloc.h"\n')
        f.write('#endif\n')

        #generate data-valid-check function list
        f.write("\n"*2)
        f.write("/* the declaration of data-valid-check function list */\n")
        f.write('extern BOOL8 '+dbname.capitalize()+'DataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue);\n\n')
        self.genDataValidCheckDeclaration(f, dbname)

        #generate data-type and c-type str-function.
        f.write("\n"*2)
        f.write('/* the declaration of '+dbname+' data-type and c-type str-functions */\n')
        f.write('extern INT8* '+dbname.capitalize()+'GetCTypeStr(UINT8 ucParaCType);\n')
        f.write('extern INT8* '+dbname.capitalize()+'GetDataTypeStr(UINT8 ucParaType);\n')

        #generate register-functions.
        f.write('\n'*2)
        f.write('/* the declaration of '+dbname+' model register functions */\n')
        f.write('extern BOOL8 '+dbname.capitalize()+'DmPatternRegisterHandler();\n')
        #f.write('extern BOOL8 '+dbname.capitalize()+'DmOpereationRegisterHandler();\n')
        #f.write('extern BOOL8 '+dbname.capitalize()+'GetRegisterHandler();\n')
        #f.write('extern BOOL8 '+dbname.capitalize()+'NotifyRegisterHandler();\n')

        #self.genExtHeaders(f)

        f.write('\n'*2)
        f.write('/* the declaration of is-object-function */\n')
        f.write('extern BOOL8 '+dbname.capitalize()+'IsObject(UINT8 ucParaType);\n')

        #write the definition of modules for write/read

        f.write('\n'*2)
        f.write('/* the declaration of convert-fuctions */\n')

        for cTypeUsed in self.cTypeUsed[dbname].keys():
            try:
                if cTypeUsed != "" and cTypeUsed != "STRUCT":
                    f.write("extern BOOL8 "+self.convertToString[cTypeUsed]+"(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue, INT8 *pcValueRange);\n")
                    f.write("extern BOOL8 "+self.convertToStruct[cTypeUsed]+"(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue, INT8 *pcValueRange);\n")
            except:
                continue

        #f.write("\n"*2)
        #f.write('/* the declaration of convert-fuctions for UINT32_BITMAP & UINT32_ENUM */\n')
        #ConvertPrefix = 'extern BOOL8 '+dbname.capitalize()
        #for tmpExtern in self.externConvertList:
        #    if tmpExtern.startswith(ConvertPrefix):
        #        f.write(tmpExtern + "\n")

        f.write("\n"*2)
        #tmpModuleItem = self.itemModuleList[dbname][0]

        self.patternName[dbname] = "g_"+dbname.capitalize()+"ModelNodes"

        f.write("DM_NODE_INFO_T "+self.patternName[dbname]+"[]=\n{\n")

        #add the last line to the cont
        tmpList = []
        tmpList.append(" "*8+"/* Name */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* RealName */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* Description */\n"+" "*8+'NULL,\n\n')
        tmpList.append(" "*8+"/* ParaType */\n"+" "*8+str(dbname).upper()+'_'+"DATA_TYPE_OBJECT,\n\n")
        tmpList.append(" "*8+"/* CType */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* sizeOffset */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* TypeSize */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* MaxElements */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* ElementCntOffset */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* ParentIndex */\n"+" "*8+"-1,\n\n")
        tmpList.append(" "*8+"/* SiblingIndex */\n"+" "*8+"-1,\n\n")
        tmpList.append(" "*8+"/* FirstChildIndex */\n"+" "*8+"-1,\n\n")
        tmpList.append(" "*8+"/* ChildrenCounter */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* ReadFlag */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* WriteFlag */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* ValueRange */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* DefaultValue */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* Supported Xml Attributes */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* Default Xml Attribute Value */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* Xml Attr-Notify Entities */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* NodeHandler */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* NotifyHandler */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* GetHandler */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* SetHandler */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* AutoSaved */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* UpgradeTransfer */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* ResetTransfer */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* BootAction */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* ValidityCheckHandler */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* OperatorDefault */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* HardwareDefault */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* Force Active Notification */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* Tr69Password */\n"+" "*8+"0,\n\n")
        tmpList.append(" "*8+"/* ToStringFunc */\n"+" "*8+"NULL,\n\n")
        tmpList.append(" "*8+"/* ToStructFunc */\n"+" "*8+"NULL\n")

        contList.append(tmpList)

        #write the contents
        for tmpCont in contList:
            tmpLine = ""
            f.write(" "*4+ "{\n")
            for i in range(0,  len(tmpCont)):
                #tmpLine +=tmpCont[i].replace('"""', '"').ljust(maxColumnList[i])
                f.write(tmpCont[i].replace('"""', '"'))
            f.write("\n"+" "*4+"},\n\n")
        f.write("};\n\n\n")

        #generate access list
        f.write('/* '+dbname+' model access list */\n')
        self.genAccessNodeList(dbname, f)

        #generate config node
        f.write("\n"*2)
        f.write('/* '+dbname+' model config node */\n')
        f.write('DB_CONFIG_T g_'+dbname.capitalize()+'ConfigNode =\n{\n')
        f.write(' '*4 +'"'+dbname+'",'+' '*8+'/* DB name */\n')

        curDbVersion = 'undefined'
        if self.dbFileVersion != '':
            curDbVersion = self.dbFileVersion
        else:
            try:
                curDbVersion = self.dbVersion[dbname]
            except:
                pass
        f.write(' '*4 +'"'+curDbVersion+'",'+' '*8+'/* DB version */\n')
        f.write(' '*4 +self.extDb[dbname]+','+' '*8+'/* if it is extended db model */\n')
        f.write(' '*4 +'"'+WorkingDir+dbname+'.xml",'+' '*8+' /* working path */\n')
        f.write(' '*4 +'"'+LoadingDir+dbname+'.xml",'+' '*8+' /* loading path */\n')
        f.write(' '*4 +'"'+BackupDir+dbname+'.xml",'+' '*8+' /* backup path */\n')
        f.write(' '*4 +'"'+MappingXmlDir+dbname+'_mapping.xml",'+' '*8+' /* cmfile-mapping path */\n')
        f.write(' '*4 +dbname.capitalize()+'DmPatternRegisterHandler,'+' '*8+' /* pattern register function */\n')
        f.write(' '*4 +dbname.capitalize()+'DataValidChecker,'+' '*8+'  /* default data-valid-check function */\n')
        f.write(' '*4 +dbname.capitalize()+'GetCTypeStr,'+' '*8+'  /* default get c-type string function */\n')
        f.write(' '*4 +dbname.capitalize()+'GetDataTypeStr,'+' '*8+'  /* default get data-type string function */\n')
        f.write(' '*4 +dbname.capitalize()+'IsObject,'+' '*8+'  /* to check if it is object */\n')
        f.write(' '*4 +'g_'+dbname.capitalize()+'DbAccessList,'+' '*8+' /* access list */\n')
        f.write(' '*4 +'NULL'+' '*8+' /* libdl.so handler */\n')
        f.write('};\n\n')

        #generate attribute pattern
        self.genAttributePattern(dbname, f)

        #generate get config-node function.
        f.write("\n"*2)
        f.write('/* get '+dbname+' model config node */\n')
        self.genGetModelConfigNode(dbname, f)

        #generate data valid-check function
        f.write("\n"*2)
        f.write('/* '+dbname+' model data-valid-check function */\n')
        self.genDataValidCheckerDefinition(dbname,f)

        #generate get-c-type string function
        f.write("\n"*2)
        f.write('/* '+dbname+' model c-type string function */\n')
        self.genCTypeStringDefinition(dbname,f)

        #generate get-data-type string function
        f.write("\n"*2)
        f.write('/* '+dbname+' model data-type string function */\n')
        self.genDataTypeStringDefinition(dbname,f)

        #generate check if it is object
        f.write("\n"*2)
        f.write('/* '+dbname+' model object-type-check function */\n')
        self.genObjectCheckDefinition(dbname,f)

        #generate register pattern func
        f.write("\n"*2)
        f.write('/* '+dbname+' model pattern register function */\n')
        self.genRegisterPatternFunc(dbname,f)

        f.close()

###end of Pattern File

    def genRegisterPatternFunc(self, dbName, f):
        funcRegName = dbName.capitalize() + "DmPatternRegisterHandler"
        f.write("BOOL8 " + funcRegName + "()\n")
        f.write("{\n")

        f.write(' '*4 + "DM_REG_PATTERN_REQ_T            *pPara;\n")
        f.write(' '*4 + "BOOL8                           bRtn;\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcRegName + "\\n\");\n")

        f.write(' '*4 + "pPara = (DM_REG_PATTERN_REQ_T *)malloc(sizeof(DM_REG_PATTERN_REQ_T));\n\n")

        f.write(' '*4 + "if(pPara == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcRegName + " malloc() failed for pPara\\n\");\n")
        f.write(' '*8 + "bRtn = DM_FALSE;\n")
        f.write(' '*8 + "return bRtn;\n")
        f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "osa_mem_set(pPara,0,sizeof(DM_REG_PATTERN_REQ_T));\n\n")

        f.write(' '*4 + "pPara->pucRootName = (INT8 *)\"" + self.rootName[dbName] + "\";\n")
        f.write(' '*4 + "pPara->pPatternFirstNode = (DM_NODE_INFO_T *)" + self.patternName[dbName] + ";\n")
        f.write(' '*4 + "pPara->lPatternSize = " + str(self.itemCnt[dbName]) + ";\n")
        f.write(' '*4 + "pPara->pAttrPatternFirstNode = (DM_ATTR_INFO_T *)" + 'g_' + dbName.capitalize() + 'Attributes;\n')

        f.write(' '*4 + "bRtn = DmRegisterPatternReq(pPara);\n")
        f.write(' '*4 + "if (!bRtn)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ DM register handler failed!\\n\");\n")
        f.write(' '*8 + "DM_FREE(pPara);\n\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "DM_FREE(pPara);\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcRegName + "\\n\");\n")

        f.write(' '*4 + "return DM_TRUE;\n")
        f.write("}\n\n")

    def genObjectCheckDefinition(self, dbname, f):
        f.write('BOOL8 '+dbname.capitalize()+'IsObject(UINT8 ucParaType)\n')
        f.write('{\n')
        f.write(' '*4+'if (ucParaType == '+str(dbname).upper()+'_'+self.dataTypeMap['object']+')\n')
        f.write(' '*4+'{\n')
        f.write(' '*8+'return DM_TRUE;\n')
        f.write(' '*4+'}\n')
        f.write(' '*4+'else\n')
        f.write(' '*4+'{\n')
        f.write(' '*8+'return DM_FALSE;\n')
        f.write(' '*4+'}\n')
        f.write('}\n')

    def genDataTypeStringDefinition(self, dbname, f):
        f.write('INT8* '+dbname.capitalize()+'GetDataTypeStr(UINT8 ucParaType)\n')
        f.write('{\n')
        f.write(' '*4+'INT8 *pTypeStr = "";\n')
        f.write('\n')
        f.write(' '*4+'switch(ucParaType)\n')
        f.write(' '*4+'{\n')

        for key in sorted(self.dataTypeMap.keys(), key=str.lower):
            f.write(' '*8+'case '+str(dbname).upper()+'_'+self.dataTypeMap[key]+':\n')

            try:
                f.write(' '*12+'pTypeStr = "'+key+'";\n')
            except:
                f.write(' '*12+'pTypeStr = "";\n')
            f.write(' '*12+'break;\n')

        f.write(' '*8+'default:\n')
        f.write(' '*12+'break;\n')
        f.write(' '*4+'}\n')
        f.write('\n')
        f.write(' '*4+'return pTypeStr;\n')
        f.write('}\n')


    def genCTypeStringDefinition(self, dbname, f):
        f.write('INT8* '+dbname.capitalize()+'GetCTypeStr(UINT8 ucParaCType)\n')
        f.write('{\n')
        f.write(' '*4+'INT8 *pTypeStr = "";\n')
        f.write('\n')
        f.write(' '*4+'switch(ucParaCType)\n')
        f.write(' '*4+'{\n')

        for key in sorted(self.cTypeUsed[dbname].keys(), key=str.lower):
            f.write(' '*8+'case '+str(dbname).upper()+'_'+self.cTypeMap[key]+':\n')

            try:
                f.write(' '*12+'pTypeStr = "'+key+'";\n')
            except:
                f.write(' '*12+'pTypeStr = "";\n')
            f.write(' '*12+'break;\n')
        f.write(' '*8+'default:\n')
        f.write(' '*12+'break;\n')
        f.write(' '*4+'}\n')
        f.write('\n')
        f.write(' '*4+'return pTypeStr;\n')
        f.write('}\n')


    def genDataValidCheckerDefinition(self, dbname, f):
        f.write('BOOL8 '+dbname.capitalize()+'DataValidChecker(UINT8 ucParaCType, INT8 *pValueRange, INT8 *pValue)\n')
        f.write('{\n')
        f.write(' '*4+'BOOL8 bRet = DM_FALSE;\n')
        f.write('\n')
        f.write(' '*4+'switch(ucParaCType)\n')
        f.write(' '*4+'{\n')

        for cTypeUsed in self.cTypeUsed[dbname].keys():
            f.write(' '*8+'case '+str(dbname).upper()+'_'+self.cTypeMap[cTypeUsed]+':\n')

            try:
                if len(self.validCheck[cTypeUsed])>0:
                    f.write(' '*12+'bRet = '+self.validCheck[cTypeUsed]+'(pValueRange, pValue);\n')
                else:
                    f.write(' '*12+'bRet = DM_TRUE;\n')
            except:
                f.write(' '*12+'bRet = DM_TRUE;\n')
            f.write(' '*12+'break;\n')
        f.write(' '*8+'default:\n')
        f.write(' '*12+'break;\n')
        f.write(' '*4+'}\n')
        f.write('\n')
        f.write(' '*4+'return bRet;\n')
        f.write('}\n')

    def genGetModelConfigNode(self, dbname, f):
        f.write('INT32 '+dbname.capitalize()+'DbLoadReq(void *pDlHandler, BOOL8 bDbReset, INT8 *pWorkPath, INT8 *pLoadPath, INT8 *pBackupPath)\n')
        f.write('{\n')
        f.write(' '*4+'return DmLoadDbReq(&g_'+dbname.capitalize()+'ConfigNode, pDlHandler, bDbReset, pWorkPath, pLoadPath, pBackupPath);\n')
        f.write('}\n')

    def genAccessNodeList(self, dbname, fc):

        fc.write('DB_ACCESS_NODE_T g_'+dbname.capitalize()+'DbAccessList[] =\n{\n')
        fc.write(' '*4+'{"ROOT", 0xffff},\n')

        tmpModuleItem = self.itemModuleList[dbname][0]

        #logger.debug("len(tmpModuleItem.accessModuleList) : "+str(len(tmpModuleItem.accessModuleList)))

        for idx in range(0, len(tmpModuleItem.accessModuleList)):
            moduleName = tmpModuleItem.accessModuleList[idx].replace(" ","_")
            #logger.debug("access node : "+moduleName)
            moduleId = tmpModuleItem.accessModuledict[moduleName]

            if moduleName != '':
                fc.write(' '*4 + '{"' + moduleName + '", ' + str(hex(moduleId)) + '},\n')

        fc.write(' '*4+'{NULL, 0}\n')
        fc.write('};\n\n')

    def genAttributePattern(self, dbname, f):
        f.write('DM_ATTR_INFO_T g_'+dbname.capitalize()+'Attributes[] =\n{\n')

        for name,description,createdbydefault,ctype,ctypesize,valuerange,defaultvalue,validcheck,tostring,tostruct in self.attributeList:
            f.write(' '*4+'{\n')
            f.write(' '*8+'/* attribute name */\n')
            f.write(' '*8+'"'+name+'",\n\n')

            f.write(' '*8+'/* attribute description */\n')
            f.write(' '*8+'"'+description+'",\n\n')

            f.write(' '*8+'/* created by default */\n')
            if createdbydefault == '':
                f.write(' '*8+'0,\n\n')
            else:
                f.write(' '*8+createdbydefault+',\n\n')

            f.write(' '*8+'/* attribute ctype */\n')
            f.write(' '*8+str(dbname).upper()+'_'+ctype+',\n\n')

            f.write(' '*8+'/* attribute ctype size */\n')
            if ctypesize.endswith('[]'):
                f.write(' '*8+'sizeof('+ctypesize[:-2]+'),\n\n')
            else:
                f.write(' '*8+'sizeof('+ctypesize+'),\n\n')

            f.write(' '*8+'/* attribute valuerange */\n')
            f.write(' '*8+'"'+valuerange+'",\n\n')

            f.write(' '*8+'/* attribute defaultvalue */\n')
            f.write(' '*8+'"'+defaultvalue+'",\n\n')

            f.write(' '*8+'/* attribute validcheck */\n')
            f.write(' '*8+validcheck+',\n\n')

            f.write(' '*8+'/* attribute tostring */\n')
            f.write(' '*8+tostring+',\n\n')

            f.write(' '*8+'/* attribute tostruct */\n')
            f.write(' '*8+tostruct+'\n\n')

            f.write(' '*4+'},\n\n')

        f.write(' '*4+'{\n')
        f.write(' '*8+'/* attribute name */\n')
        f.write(' '*8+'NULL,\n\n')

        f.write(' '*8+'/* attribute description */\n')
        f.write(' '*8+'NULL,\n\n')

        f.write(' '*8+'/* created by default */\n')
        f.write(' '*8+'0,\n\n')

        f.write(' '*8+'/* attribute ctype */\n')
        f.write(' '*8+'0,\n\n')

        f.write(' '*8+'/* attribute ctype size */\n')
        f.write(' '*8+'0,\n\n')

        f.write(' '*8+'/* attribute valuerange */\n')
        f.write(' '*8+'NULL,\n\n')

        f.write(' '*8+'/* attribute defaultvalue */\n')
        f.write(' '*8+'NULL,\n\n')

        f.write(' '*8+'/* attribute validcheck */\n')
        f.write(' '*8+'NULL,\n\n')

        f.write(' '*8+'/* attribute tostring */\n')
        f.write(' '*8+'NULL,\n\n')

        f.write(' '*8+'/* attribute tostruct */\n')
        f.write(' '*8+'NULL\n\n')

        f.write(' '*4+'}\n\n')

        f.write('};\n\n')


###begin of Register File

    def genDbCbsHeaderFile(self, moduleName, headerFile):

        f = open(headerFile, 'wb')

        f.write('\n'*2)
        f.write("#ifndef DM_"+str(moduleName).upper()+"_REGISTER_DB_CBS_HANDLER_H\n")
        f.write("#define DM_"+str(moduleName).upper()+"_REGISTER_DB_CBS_HANDLER_H\n")

        f.write('\n'*2)
        f.write("/* declaration of reigster functions */\n")

        f.write('\n'*2)
        f.write("/* inline fuction */\n")
        f.write("inline BOOL8 "+str(moduleName).capitalize()+"RegisterDbCbsHandler(void)\n")
        f.write("{\n")
        f.write(' '*4+'/* call reigster functions here*/\n')
        f.write('\n')
        f.write(' '*4+'return DM_TRUE;\n')
        f.write('}\n')

        f.write('\n'*2)
        f.write("#endif /* DM_"+str(moduleName).upper()+"_REGISTER_DB_CBS_HANDLER_H */\n")

        f.close()

    def addDbCbToHeaderFile(self, headerFile, funcName):

        f = open(headerFile, 'rb')

        allContent = f.read()

        f.close()

        os.remove(headerFile)

        #find declaration and insert funcName
        inIndex = allContent.find('/* declaration of reigster functions */\n')
        if not inIndex == -1:
            allContent = allContent[:inIndex+len('/* declaration of reigster functions */\n')]+'extern BOOL8 '+funcName+'(void);\n'+allContent[inIndex+len('/* declaration of reigster functions */\n'):]
        #find definition and insert funcName
        inIndex = allContent.find('/* call reigster functions here*/\n')
        if not inIndex == -1:
            allContent = allContent[:inIndex+len('/* call reigster functions here*/\n')]+' '*4+'if (DM_FALSE=='+funcName+'()) { return DM_FALSE; }\n'+allContent[inIndex+len('/* call reigster functions here*/\n'):]

        f = open(headerFile, 'wb')
        f.write(allContent)
        f.close()

    def genRegisterGetHandleFile(self, filePath, dbName, modelhdr, getMod):

        getListNotEmpty = 0
        tmpGetList = []
        for tmpItem in self.itemList[dbName]:
            if tmpItem.getHandlerList[getMod] == "":
                continue
            else:
                tmpGetList.append(tmpItem.getHandlerList[getMod])
                getListNotEmpty = 1

        if getListNotEmpty==0:
            return

        fileName = str(getMod).lower() + "_" + str(dbName).lower() + "_register_get_handler.c"
        funcName = getMod.capitalize() + dbName.capitalize() + "GetRegisterHandler"

        if not os.path.exists(ResultSrcDir + "/db_cbs/" + str(getMod).lower() + "/"):
            os.makedirs(ResultSrcDir + "/db_cbs/" + str(getMod).lower() + "/")

        cbsHeaderFile = ResultSrcDir + "/db_cbs/" + str(getMod).lower() + "/" + str(getMod).lower() + "_db_cbs.h"
        if not os.path.exists(cbsHeaderFile):
            self.genDbCbsHeaderFile(str(getMod).lower(), cbsHeaderFile)

        self.addDbCbToHeaderFile(cbsHeaderFile,funcName)

        logger.debug('writing '+ResultSrcDir + "/db_cbs/" + str(getMod).lower() + "/" + fileName)
        f=open(ResultSrcDir + "/db_cbs/" + str(getMod).lower() + "/" + fileName,  'wb')

        f.write('\n'*2)
        f.write("#include \"dm_internal.h\"\n")
        #f.write('#include "'+modelhdr+'"\n\n')

        f.write('#ifdef DMALLOC_CHECK_ENABLED\n')
        f.write('#warning ***** compilation with dmalloc *****\n')
        f.write('#include "dmalloc.h"\n')
        f.write('#endif\n')

        tmpDictList = set(tmpGetList)

        for tmpFunc in tmpDictList:
            f.write("extern INT32 " + tmpFunc + "(DM_GET_VALUE_T *pGetValue);\n")

        f.write("\n")

        ##write note
        f.write("/*******************************************************************************\n" )
        f.write("  Module   : " + funcName + "                                                   \n" )
        f.write("  Function : register get handler                                               \n" )
        f.write("  Input    : void                                                               \n" )
        f.write("  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          \n" )
        f.write("  Note     : null                                                               \n" )
        f.write("*******************************************************************************/\n" )
        f.write("BOOL8 " + funcName + "(void)\n")
        f.write("{\n")

        f.write(' '*4 + "DM_REG_GET_HANDLER_PARA_T       *pPara;\n")
        f.write(' '*4 + "BOOL8                           bRtn = DM_TRUE;\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\");\n")

        f.write(' '*4 + "pPara = (DM_REG_GET_HANDLER_PARA_T *)malloc(sizeof(DM_REG_GET_HANDLER_PARA_T));\n\n")
        f.write(' '*4 + "if(pPara == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pPara\");\n")
        f.write(' '*8 + "bRtn = DM_FALSE;\n")
        f.write(' '*8 + "return bRtn;\n")
        f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "memset(pPara, 0, sizeof(DM_REG_GET_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "pPara->lParaCnt = 1;\n")
        f.write(' '*4 + "pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);\n\n")
        f.write(' '*4 + "if(pPara->paParaPath == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pPara->paParaPath\");\n")
        f.write(' '*8 + "DM_FREE(pPara);\n")
        f.write(' '*8 + "bRtn = DM_FALSE;\n")
        f.write(' '*8 + "return bRtn;\n")
        f.write(' '*4 + "}\n\n")
        f.write(' '*4 + "memset(pPara->paParaPath, 0, pPara->lParaCnt);\n\n")

        ##insert the register function
        for tmpItem in self.itemList[dbName]:
            if tmpItem.getHandlerList[getMod] == "":
                continue
            else:
                f.write(' '*4 + "/* Register Get " + tmpItem.nodeFullName + " Handler. */\n")
                f.write(' '*4 + "pPara->paParaPath[0] = (INT8*)\"" + tmpItem.nodeFullNameReplace + "\";\n")
                f.write(' '*4 + "pPara->pCallbackFunc = (DM_GET_CALLBACK_FUN)" + tmpItem.getHandlerList[getMod] + ";\n")
                f.write(' '*4 + "bRtn = DmRegisterGetHandlerReq(pPara);\n")
                f.write(' '*4 + "if (!bRtn)\n")
                f.write(' '*4 + "{\n")
                f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ Para %s get register handler failed!\", pPara->paParaPath[0]);\n")
                f.write(' '*8 + "DM_FREE(pPara->paParaPath);\n")
                f.write(' '*8 + "DM_FREE(pPara);\n")
                f.write(' '*8 + "return DM_FALSE;\n")
                f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "DM_FREE(pPara->paParaPath);\n")
        f.write(' '*4 + "DM_FREE(pPara);\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\");\n")

        f.write(' '*4 + "return bRtn;\n")
        f.write("}\n")

        f.write("\n")

        f.close()

    def genRegisterSetHandleFile(self, filePath, dbName, modelhdr, setMod):

        setListNotEmpty = 0
        tmpSetList = []
        for tmpItem in self.itemList[dbName]:
            if tmpItem.setHandlerList[setMod] == "":
                continue
            else:
                tmpSetList.append(tmpItem.setHandlerList[setMod])
                setListNotEmpty = 1

        if setListNotEmpty==0:
            return

        fileName = str(setMod).lower() + "_" + str(dbName).lower() + "_register_set_handler.c"
        funcName = setMod.capitalize() + dbName.capitalize() + "SetRegisterHandler"

        if not os.path.exists(ResultSrcDir + "/db_cbs/" + str(setMod).lower() + "/"):
            os.makedirs(ResultSrcDir + "/db_cbs/" + str(setMod).lower() + "/")

        cbsHeaderFile = ResultSrcDir + "/db_cbs/" + str(setMod).lower() + "/" + str(setMod).lower() + "_db_cbs.h"
        if not os.path.exists(cbsHeaderFile):
            self.genDbCbsHeaderFile(str(setMod).lower(), cbsHeaderFile)

        self.addDbCbToHeaderFile(cbsHeaderFile,funcName)

        logger.debug('writing '+ResultSrcDir + "/db_cbs/" + str(setMod).lower() + "/" + fileName)
        f=open(ResultSrcDir + "/db_cbs/" + str(setMod).lower() + "/" + fileName,  'wb')

        f.write('\n'*2)
        f.write("#include \"dm_internal.h\"\n")
        #f.write('#include "'+modelhdr+'"\n\n')

        f.write('#ifdef DMALLOC_CHECK_ENABLED\n')
        f.write('#warning ***** compilation with dmalloc *****\n')
        f.write('#include "dmalloc.h"\n')
        f.write('#endif\n')

        tmpDictList = set(tmpSetList)

        for tmpFunc in tmpDictList:
            f.write("extern INT32 " + tmpFunc + "(DM_SET_VALUE_T *pGetValue);\n")

        f.write("\n")

        ##write note
        f.write("/*******************************************************************************\n" )
        f.write("  Module   : " + funcName + "                                                   \n" )
        f.write("  Function : register set handler                                               \n" )
        f.write("  Input    : void                                                               \n" )
        f.write("  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          \n" )
        f.write("  Note     : null                                                               \n" )
        f.write("*******************************************************************************/\n" )
        f.write("BOOL8 " + funcName + "(void)\n")
        f.write("{\n")

        f.write(' '*4 + "DM_REG_SET_HANDLER_PARA_T       *pPara;\n")
        f.write(' '*4 + "BOOL8                           bRtn = DM_TRUE;\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\");\n")

        f.write(' '*4 + "pPara = (DM_REG_SET_HANDLER_PARA_T *)malloc(sizeof(DM_REG_SET_HANDLER_PARA_T));\n\n")
        f.write(' '*4 + "if(pPara == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pPara\");\n")
        f.write(' '*8 + "bRtn = DM_FALSE;\n")
        f.write(' '*8 + "return bRtn;\n")
        f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "memset(pPara, 0, sizeof(DM_REG_SET_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "pPara->lParaCnt = 1;\n")
        f.write(' '*4 + "pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);\n\n")
        f.write(' '*4 + "if(pPara->paParaPath == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pPara->paParaPath\");\n")
        f.write(' '*8 + "DM_FREE(pPara);\n")
        f.write(' '*8 + "bRtn = DM_FALSE;\n")
        f.write(' '*8 + "return bRtn;\n")
        f.write(' '*4 + "}\n\n")
        f.write(' '*4 + "memset(pPara->paParaPath, 0, pPara->lParaCnt);\n\n")

        ##insert the register function
        for tmpItem in self.itemList[dbName]:
            if tmpItem.setHandlerList[setMod] == "":
                continue
            else:
                f.write(' '*4 + "/* Register Set " + tmpItem.nodeFullName + " Handler. */\n")
                f.write(' '*4 + "pPara->paParaPath[0] = (INT8*)\"" + tmpItem.nodeFullNameReplace + "\";\n")
                f.write(' '*4 + "pPara->pCallbackFunc = (DM_SET_CALLBACK_FUN)" + tmpItem.setHandlerList[setMod] + ";\n")
                f.write(' '*4 + "bRtn = DmRegisterSetHandlerReq(pPara);\n")
                f.write(' '*4 + "if (!bRtn)\n")
                f.write(' '*4 + "{\n")
                f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ Para %s set register handler failed!\", pPara->paParaPath[0]);\n")
                f.write(' '*8 + "DM_FREE(pPara->paParaPath);\n")
                f.write(' '*8 + "DM_FREE(pPara);\n")
                f.write(' '*8 + "return DM_FALSE;\n")
                f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "DM_FREE(pPara->paParaPath);\n")
        f.write(' '*4 + "DM_FREE(pPara);\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\");\n")

        f.write(' '*4 + "return bRtn;\n")
        f.write("}\n")

        f.write("\n")
        f.close()

    def genRegisterNotifyHandleFile(self, filePath, dbName, modelhdr, notifyMod):

        notifyListNotEmpty = 0
        tmpNotifyList = []
        for tmpItem in self.itemList[dbName]:
            if tmpItem.notifyHandlerlist[notifyMod] == "":
                continue
            else:
                tmpNotifyList.append(tmpItem.notifyHandlerlist[notifyMod])
                notifyListNotEmpty = 1

        if notifyListNotEmpty==0:
            return

        fileName = str(notifyMod).lower() + "_" + str(dbName).lower() + "_register_notify_handler.c"
        funcName = notifyMod.capitalize() + dbName.capitalize() + "NotifyRegisterHandler"

        if not os.path.exists(ResultSrcDir + "/db_cbs/" + str(notifyMod).lower() + "/"):
            os.makedirs(ResultSrcDir + "/db_cbs/" + str(notifyMod).lower() + "/")

        cbsHeaderFile = ResultSrcDir + "/db_cbs/" + str(notifyMod).lower() + "/" + str(notifyMod).lower() + "_db_cbs.h"
        if not os.path.exists(cbsHeaderFile):
            self.genDbCbsHeaderFile(str(notifyMod).lower(), cbsHeaderFile)

        self.addDbCbToHeaderFile(cbsHeaderFile,funcName)

        logger.debug('writing '+ResultSrcDir + "/db_cbs/" + str(notifyMod).lower() + "/" + fileName)
        f=open(ResultSrcDir + "/db_cbs/" + str(notifyMod).lower() + "/" + fileName,  'wb')

        f.write('\n'*2)
        f.write("#include \"dm_internal.h\"\n")
        #f.write('#include "'+modelhdr+'"\n\n')

        f.write('#ifdef DMALLOC_CHECK_ENABLED\n')
        f.write('#warning ***** compilation with dmalloc *****\n')
        f.write('#include "dmalloc.h"\n')
        f.write('#endif\n')

        tmpDictList = set(tmpNotifyList)

        for tmpFunc in tmpDictList:
            f.write("extern void " + tmpFunc + "(DM_NOTIFY_T *pPara);\n")

        f.write("\n")

        f.write("/*******************************************************************************\n" )
        f.write("  Module   : " + funcName + "                                                   \n" )
        f.write("  Function : register notify handler                                            \n" )
        f.write("  Input    : void                                                               \n" )
        f.write("  Ounput   : BOOL8: DM_TRUE/DM_FALSE                                          \n" )
        f.write("  Note     : null                                                               \n" )
        f.write("*******************************************************************************/\n" )
        f.write("BOOL8 " + funcName + "(void)\n")
        f.write("{\n")

        f.write(' '*4 + "DM_NOTIFY_HANDLER_REG_PARA_T    *pPara;\n")
        f.write(' '*4 + "BOOL8                           bRtn = DM_TRUE;\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\");\n")

        f.write(' '*4 + "pPara = (DM_NOTIFY_HANDLER_REG_PARA_T *)malloc(sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));\n\n")

        f.write(' '*4 + "if(pPara == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pPara\");\n")
        f.write(' '*8 + "bRtn = DM_FALSE;\n")
        f.write(' '*8 + "return bRtn;\n")
        f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "memset(pPara, 0, sizeof(DM_NOTIFY_HANDLER_REG_PARA_T));\n\n")

        f.write(' '*4 + "pPara->lParaCnt = 1;\n")
        f.write(' '*4 + "pPara->paParaPath = (INT8 **)malloc(sizeof(INT8 *)*pPara->lParaCnt);\n\n")

        f.write(' '*4 + "if(pPara->paParaPath == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pPara->paParaPath\");\n")
        f.write(' '*8 + "DM_FREE(pPara);\n")
        f.write(' '*8 + "bRtn = DM_FALSE;\n")
        f.write(' '*8 + "return bRtn;\n")
        f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "memset(pPara->paParaPath, 0, pPara->lParaCnt);\n\n")

        ##insert the register function
        for tmpItem in self.itemList[dbName]:
            if tmpItem.notifyHandlerlist[notifyMod] == "":
                continue
            else:
                f.write(' '*4 + "//Register Notify " + tmpItem.nodeFullName + " Handler.\n")
                f.write(' '*4 + "pPara->paParaPath[0] = (INT8*)\"" + tmpItem.nodeFullNameReplace + "\";\n")
                f.write(' '*4 + "pPara->pCallbackFunc = (DM_NOTIFY_CALLBACK_FUN)" + tmpItem.notifyHandlerlist[notifyMod] + ";\n")
                f.write(' '*4 + "bRtn = DmRegisterNotifyHandlerReq(pPara);\n")
                f.write(' '*4 + "if (!bRtn)\n")
                f.write(' '*4 + "{\n")
                f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ Para %s notify register handler failed!\\n\", pPara->paParaPath[0]);\n")
                f.write(' '*8 + "DM_FREE(pPara->paParaPath);\n")
                f.write(' '*8 +"DM_FREE(pPara);\n")
                f.write(' '*8 + "return DM_FALSE;\n")
                f.write(' '*4 + "}\n\n")

        f.write(' '*4 +"DM_FREE(pPara->paParaPath);\n")
        f.write(' '*4 +"DM_FREE(pPara);\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\");\n")

        f.write(' '*4 +"return bRtn;\n")
        f.write("}\n")

        f.close()

    def genRegisterFile(self, filePath, dbName, modelhdr):
        """
        generate the regist files:
        """

        #generate the register files

        tmpModule = self.itemModuleList[dbName][0]

        if self.extDb[dbName] == 'DM_FALSE':
            cbMods = {}
            for getMod in tmpModule.getModuleList:
                self.genRegisterGetHandleFile(filePath, dbName, modelhdr, getMod)
                try:
                    cbMods[getMod]=1
                except:
                    pass

            for setMod in tmpModule.setModuleList:
                self.genRegisterSetHandleFile(filePath, dbName, modelhdr, setMod)
                try:
                    cbMods[setMod]=1
                except:
                    pass

            for notifyMod in tmpModule.notifyModuleList:
                self.genRegisterNotifyHandleFile(filePath, dbName, modelhdr, notifyMod)
                try:
                    cbMods[notifyMod]=1
                except:
                    pass

            for key in cbMods.keys():
                self.genDbCbMakeFile(dbName,key.strip(),"all")

###end of Register File

###begin of Operation File

    def convertBitmapToString(self, f, item):
        funcName = item.nodeFullName.replace(".", "_") + "_DmConvertBitmapToString"
        f.write("BOOL8 "+ funcName + "(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue)\n")
        f.write("{\n")
        f.write(' '*4 +"INT32   *pTmp = (INT32 *)pStructValue;\n")
        f.write(' '*4 +"INT8    *pDest = (INT8 *)pucStringValue;\n")
        f.write(' '*4 +"INT8    *cDelimiters = \",\";\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\\n\");\n")

        f.write(' '*4 + "if (lStructCnt != 1)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ input parameters error!\\n\");\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n")

        f.write(' '*4 + "*plStringCnt = lStructCnt;\n\n")

        bitmapArray = item.valueRange.split("\n")
        for tmpBitmap in bitmapArray:
            f.write(' '*4 + "if ((*pTmp) & " + item.nodeFullName.replace(".", "_").upper() + "_" + tmpBitmap.replace(" ","_").replace("-","_").replace("/","_").replace("*","X").upper() + ")\n")
            f.write(' '*4 + "{\n")
            f.write(' '*8 + "if (strcmp(pDest,\"\") != 0)\n")
            f.write(' '*8 + "{\n")
            f.write(' '*12 + "strcat(pDest, cDelimiters);\n")
            f.write(' '*8 + "}\n")
            f.write(' '*8 + "strcat(pDest, \"" + tmpBitmap + "\");\n")
            f.write(' '*4 + "}\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\\n\");\n")

        f.write(' '*4 +"return DM_TRUE;\n")
        f.write("}\n\n")

    def convertStringToBitmap(self, f, item):
        funcName = item.nodeFullName.replace(".", "_") + "_DmConvertStringToBitmap"
        f.write("BOOL8 "+ funcName + "(INT32 lStringCnt, void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue)\n")
        f.write("{\n")
        f.write(' '*4 +"INT8    *pBitmap = NULL;\n")
        f.write(' '*4 +"INT8    *pTmp = (INT8 *)pucStringValue;\n")
        f.write(' '*4 +"INT32   *pDest = (INT32 *)pStructValue;\n")
        f.write(' '*4 +"INT8    *cSaveptr = NULL;\n")
        f.write(' '*4 +"INT8    *cDelimiters = \",\";\n\n")

        f.write(' '*4 +"*pDest = 0;\n")
        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\\n\");\n")

        f.write(' '*4 + "if (lStringCnt == 0)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ empty parameter!\\n\");\n")
        f.write(' '*8 + "return DM_TRUE;\n")
        f.write(' '*4 + "}\n")

        f.write(' '*4 + "*pulStructCnt = lStringCnt;\n")

        f.write(' '*4 + "pBitmap = strtok_r (pTmp,cDelimiters,&cSaveptr);\n")
        f.write(' '*4 + "while (pBitmap != NULL)\n")
        f.write(' '*4 + "{\n")

        bitmapArray = item.valueRange.split("\n")
        for tmpBitmap in bitmapArray:
            f.write(' '*8 + "if (strcmp(pBitmap, \"" + tmpBitmap + "\") == 0)\n")
            f.write(' '*8 + "{\n")
            f.write(' '*12 + "*pDest |= " + item.nodeFullName.replace(".", "_").upper() + "_" + tmpBitmap.replace(" ","_").replace("-","_").replace("/","_").replace("*","X").upper() + ";\n")
            f.write(' '*8 + "}\n")

        f.write("\n")
        f.write(' '*8 + "pBitmap = strtok_r (NULL,cDelimiters,&cSaveptr);\n")
        f.write(' '*4 + "}\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\\n\");\n")

        f.write(' '*4 +"return DM_TRUE;\n")
        f.write("}\n\n")

    def convertEnumToString(self, f, item):
        funcName = item.nodeFullName.replace(".", "_") + "_DmConvertEnumToString"
        f.write("BOOL8 "+ funcName + "(INT32 lStructCnt, void *pStructValue, INT32 *plStringCnt, void *pucStringValue)\n")
        f.write("{\n")
        f.write(' '*4 +"INT32   *ptmp = (INT32 *)pStructValue;\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\\n\");\n")

        f.write(' '*4 + "if (lStructCnt != 1)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ input parameters error!\\n\");\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n")

        f.write(' '*4 + "*plStringCnt = lStructCnt;\n\n")

        enumArray = item.valueRange.split("\n")
        for tmpEnum in enumArray:
            f.write(' '*4 + "if (*ptmp == " + item.nodeFullName.replace(".", "_").upper() + "_" + tmpEnum.replace(" ","_").replace("-","_").replace("/","_").replace("*","X").upper() + ")\n")
            f.write(' '*4 + "{\n")
            f.write(' '*8 + "strcpy(pucStringValue, (INT8*)\"" + tmpEnum + "\");\n")
            f.write(' '*8 + "return DM_TRUE;\n")
            f.write(' '*4 + "}\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\\n\");\n")

        f.write(' '*4 +"return DM_FALSE;\n")
        f.write("}\n\n")

    def convertStringToEnum(self, f, item):
        funcName = item.nodeFullName.replace(".", "_") + "_DmConvertStringToEnum"
        f.write("BOOL8 "+ funcName + "(INT32 lStringCnt,  void *pucStringValue, UINT32 *pulStructCnt, void *pStructValue)\n")
        f.write("{\n")
        f.write(' '*4 +"INT8   *pEnumString = (INT8 *)pucStringValue;\n\n")
        f.write(' '*4 +"INT32   *pTmp = (INT32 *)pStructValue;\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\");\n")

        f.write(' '*4 + "if (lStringCnt == 0)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ input parameters error!\\n\");\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n")

        f.write(' '*4 + "*pulStructCnt = lStringCnt;\n\n")

        enumArray = item.valueRange.split("\n")
        for tmpEnum in enumArray:
            f.write(' '*4 + "if (strcmp(pEnumString, (INT8*)\"" + tmpEnum + "\") == 0)\n")
            f.write(' '*4 + "{\n")
            f.write(' '*8 + "*pTmp = " + item.nodeFullName.replace(".", "_").upper() + "_" + tmpEnum.replace(" ","_").replace("-","_").replace("/","_").replace("*","X").upper() + ";\n")
            f.write(' '*8 + "return DM_TRUE;\n")
            f.write(' '*4 + "}\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\\n\");\n")

        f.write(' '*4 +"return DM_FALSE;\n")
        f.write("}\n\n")

    def genAddFunc(self,f,item,dbname):
        funcName = "Add_" + item.nodeFullName.replace(".", "_") + "Req"

        item.operationFunc["add"] = funcName

        f.write("INT32 "+ funcName + "(DM_ADD_OBJ_BY_ID_REQ_T *pAddPara)\n")
        f.write("{\n")

        f.write(' '*4 + "DM_HANDLER_PARA_T  *pDmHandlerPara;\n")
        f.write(' '*4 + item.struction + "  *pDmHandlerValue;\n")
        for index in range(item.selfIndex+1, self.maxIndex[dbname]):
            if self.itemList[dbname][index].parentIndex == item.selfIndex:
                if self.itemList[dbname][index].isMultObj:
                    f.write(' '*4 + "UINT32              i;\n")
                    break

        f.write(' '*4 + "BOOL8              bRtn;\n\n")



        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\\n\");\n")

        f.write(' '*4 + "pDmHandlerPara = (DM_HANDLER_PARA_T *)malloc(sizeof(DM_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "if (pDmHandlerPara == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pDmHandlerPara\\n\");\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*8 + "}\n\n")

        f.write(' '*4 + "memset(pDmHandlerPara, 0, sizeof(DM_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "pDmHandlerPara->pDbHandler = GetDmHandlerByName(pAddPara->pucPath);\n")
        f.write(' '*4 + "if (pDmHandlerPara->pDbHandler == NULL)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ Para %s GetDmHandlerByName failed!\\n\", pAddPara->pucPath);\n")
        f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n")
        f.write(' '*4 + "pDmHandlerPara->TransactionId = pAddPara->TransactionId;\n")
        f.write(' '*4 + "pDmHandlerPara->bCInterface = DM_TRUE;\n")
        f.write(' '*4 + "pDmHandlerPara->ulCnt = 1;\n\n")

        f.write(' '*4 + "pDmHandlerPara->pucName = (INT8 *)malloc(1024);\n")
        f.write(' '*4 + "memset(pDmHandlerPara->pucName, 0, 1024);\n\n")
        f.write(' '*4 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pAddPara->pucPath);\n\n")

        f.write(' '*4 + "pDmHandlerValue = (" + item.struction + " *)pAddPara->pObject;\n\n")

        for index in range(item.selfIndex+1, self.maxIndex[dbname]):
##            if self.itemList[index].operationFunc["add"] == "":
##                f.write(' '*4 + "//!!! can't find the sub function of " + self.itemList[index].nodeFullName + " in add function.\n")
##                continue
            if self.itemList[dbname][index].parentIndex == item.selfIndex:

                f.write(' '*4 + "pDmHandlerPara->ulParaId = " + self.itemList[dbname][index].definition + ";\n")
                f.write(' '*4 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \"" + self.itemList[dbname][index].nameReplace + "\");\n")
                if self.itemList[dbname][index].isObject:
                    f.write(' '*4 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \".\");\n")
                    #if self.itemList[index].isMultObj:
                        #f.write(' '*4 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \"{i}.\");\n")
                    f.write(' '*4 + "pDmHandlerPara->bItem = DM_FALSE;\n")
                else:
                    f.write(' '*4 + "pDmHandlerPara->bItem = DM_TRUE;\n")

                if not self.itemList[dbname][index].isMultObj:
                    f.write(' '*4 + "pDmHandlerPara->pValue = (INT8 *)&(pDmHandlerValue->" + self.itemList[dbname][index].name + ");\n")
                    f.write(' '*4 + "pDmHandlerPara->valueSize = sizeof(pDmHandlerValue->" + self.itemList[dbname][index].name + ");\n")

                if self.itemList[dbname][index].cType[-2:] == "[]":
                    f.write(' '*4 + "pDmHandlerPara->ulCnt = pDmHandlerValue->"+ self.itemList[dbname][index].name +"Cnt;\n")
                else:
                    f.write(' '*4 + "pDmHandlerPara->ulCnt = 1;\n")

                f.write("\n")
                if self.itemList[dbname][index].isMultObj:
                    f.write(' '*4 + "for(i = 0; i < pDmHandlerValue->"+self.itemList[dbname][index].name+"_cnt; i++)\n")
                    f.write(' '*4 + "{\n")
                    f.write(' '*8 + "pDmHandlerPara->pValue = (INT8 *)&(pDmHandlerValue->" + self.itemList[dbname][index].name + "[i]);\n")
                    f.write(' '*8 + "pDmHandlerPara->valueSize = sizeof(pDmHandlerValue->" + self.itemList[dbname][index].name + "[i]);\n")
                    f.write(' '*8 + "bRtn = DmCallDefaultAddHandler(pDmHandlerPara);\n")
                    f.write(' '*8 + "if (!bRtn)\n")
                    f.write(' '*8 + "{\n")
                    f.write(' '*12 + "DM_FREE(pDmHandlerPara);\n")
                    f.write(' '*12 + "return DM_FALSE;\n")
                    f.write(' '*8 + "}\n")
                    f.write(' '*4 + "}\n")
                else:
                    f.write(' '*4 + "bRtn = DmCallDefaultAddHandler(pDmHandlerPara);\n")
                    f.write(' '*4 + "if (!bRtn)\n")
                    f.write(' '*4 + "{\n")
                    f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
                    f.write(' '*8 + "return DM_FALSE;\n")
                    f.write(' '*4 + "}\n")

                f.write(' '*4 + "memset(pDmHandlerPara->pucName, 0, 1024);\n")
                f.write(' '*4 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pAddPara->pucPath);\n\n")

        f.write(' '*4 + "DM_FREE(pDmHandlerPara->pucName);\n")
        f.write(' '*4 + "DM_FREE(pDmHandlerPara);\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\\n\");\n")

        f.write(' '*4 + "return DM_TRUE;\n")
        f.write("}\n\n")

    def genGetFunc(self,f,item,dbname):
        if item.isObject:
            funcName = "Get_" + item.nodeFullName.replace(".", "_") + "Req"
        else:
            funcName = "Get_" + item.nodeFullName.replace(".", "_") + "_Req"

        item.operationFunc["get"] = funcName

        f.write("INT32 "+ funcName + "(DM_GET_PARA_C_VALUE_REQ_T *pGetPara)\n")
        f.write("{\n")

        f.write(' '*4 + "DM_HANDLER_PARA_T  *pDmHandlerPara;\n")
        f.write(' '*4 + item.struction + "  *pDmHandlerValue;\n")
        if item.isObject:
            f.write(' '*4 + "UINT32              i;\n")
        f.write(' '*4 + "BOOL8              bRtn;\n\n")

        if item.isMultObj:
            f.write(' '*4 + "xmlXPathObjectPtr   xpathObj = NULL;\n")
            f.write(' '*4 + "xmlNodePtr          nodeBak = NULL;\n")
            f.write(' '*4 + "BOOL8               bInstance;\n")
            f.write(' '*4 + "INT8                acNodeName[16] = {0};\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\\n\");\n")

        f.write(' '*4 + "pDmHandlerPara = (DM_HANDLER_PARA_T *)malloc(sizeof(DM_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "if (pDmHandlerPara == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pDmHandlerPara\\n\");\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "memset(pDmHandlerPara, 0, sizeof(DM_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "pDmHandlerPara->pDbHandler = GetDmHandlerByName(pGetPara->pucPath);\n")
        f.write(' '*4 + "if (pDmHandlerPara->pDbHandler == NULL)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ Para %s GetDmHandlerByName failed!\\n\", pGetPara->pucPath);\n")
        f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n")
        f.write(' '*4 + "pDmHandlerPara->TransactionId = pGetPara->TransactionId;\n")
        f.write(' '*4 + "pDmHandlerPara->bCInterface = DM_TRUE;\n\n")
        #f.write(' '*4 + "pDmHandlerPara->ulCnt = 1;\n\n") # cyq:I don't think it's needed

        f.write(' '*4 + "pDmHandlerPara->pucName = (INT8 *)malloc(1024);\n")
        f.write(' '*4 + "memset(pDmHandlerPara->pucName, 0, 1024);\n\n")
        f.write(' '*4 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pGetPara->pucPath);\n\n")

        f.write(' '*4 + "pDmHandlerValue = (" + item.struction + " *)pGetPara->pValue;\n\n")

        if item.isObject:
            #pasre the object
            if item.isMultObj:
                f.write(' '*4 + "UINT32  alIndex[" + item.nodeFullName.replace(".", "_").upper() + "CNT + 1] = {0};\n")
                f.write(' '*4 + "INT8 *pucTmpGetParaPath = (INT8 *)calloc(1024, sizeof(UINT8));\n")

                f.write(' '*4 + "if(pucTmpGetParaPath == NULL)\n")
                f.write(' '*4 + "{\n")

                f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pDmHandlerPara\\n\");\n")
                f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
                f.write(' '*8 + "return DM_FALSE;\n")
                f.write(' '*4 + "}\n\n")

                f.write(' '*4 + "bInstance = DmIsSingleOfMultiple((INT8 *)pGetPara->pucPath);\n")
                f.write(' '*4 + "if(DM_TRUE == bInstance)\n")
                f.write(' '*4 + "{\n")
                f.write(' '*8 + "pGetPara->ulCnt = 1;\n")
                f.write(' '*8 + "alIndex[0] = 1;\n")
                f.write(' '*8 + "alIndex[1] = DmGetInstanceNumFromPath((INT8 *)pGetPara->pucPath);\n")
                f.write(' '*8 + "memset(pDmHandlerPara->pucName, 0, 1024);\n\n")
                f.write(' '*8 + "strncpy(pucTmpGetParaPath, pGetPara->pucPath, DmGetObjectLengthOfInstance(pGetPara->pucPath)); \n")
                f.write(' '*4 + "}\n")
                f.write(' '*4 + "else\n")
                f.write(' '*4 + "{\n")
                f.write(' '*8 + "strcpy(pucTmpGetParaPath, pGetPara->pucPath);\n")
                f.write(' '*8 + "bRtn = DbGetObjectIndexCont(pDmHandlerPara->pDbHandler, pGetPara->pucPath, alIndex);\n")
                f.write(' '*8 + "if (!bRtn)\n")
                f.write(' '*8 + "{\n")
                f.write(' '*12 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ Para %s DbGetObjectIndexCont failed!\\n\", pGetPara->pucPath);\n")
                f.write(' '*12 + "DM_FREE(pucTmpGetParaPath);\n")
                f.write(' '*12 + "DM_FREE(pDmHandlerPara->pucName);\n")
                f.write(' '*12 + "DM_FREE(pDmHandlerPara);\n")
                f.write(' '*12 + "return DM_FALSE;\n")
                f.write(' '*8 + "}\n")
                f.write(' '*8 + "pGetPara->ulCnt = alIndex[0];\n")
                f.write(' '*8 + "memset(pDmHandlerPara->pucName, 0, 1024);\n")
                f.write(' '*4 + "}\n")
            else:
                f.write(' '*4 + "pGetPara->ulCnt = 1;\n")

            f.write(' '*4 + "for(i=0;i<pGetPara->ulCnt;i++)\n")
            f.write(' '*4 + "{\n")

            if item.isMultObj:    #generate clause for get instanceNum
                f.write(' '*8 + "if(DM_FALSE == bInstance)\n")
                f.write(' '*8 + "{\n")
                f.write(' '*12 + "nodeBak = pDmHandlerPara->pDbHandler->xpathCtx->node;\n")
                f.write(' '*12 + "memset(acNodeName,0,sizeof(INT8)*16);\n")
                f.write(' '*12 + "sprintf(acNodeName,\"%s%d\",\"i\", alIndex[i+1]);\n")
                f.write(' '*12 + "xpathObj = DbGetXPathObjPtr(pDmHandlerPara->pDbHandler->xpathCtx, (INT8 *)acNodeName);\n")
                f.write(' '*8 + "}\n")
                f.write(' '*8 + "pDmHandlerValue[i].instanceNum = alIndex[i+1];\n")
            for index in range(item.selfIndex+1, self.maxIndex[dbname]):
##                if self.itemList[index].operationFunc["get"] == "":
##                    f.write(' '*8 + "//!!! can't find the sub function of " + self.itemList[index].nodeFullName + " in get function.\n")
##                    continue
                if self.itemList[dbname][index].parentIndex == item.selfIndex:

                    if item.isMultObj:
                        f.write(' '*8 + "strcat((INT8*)pDmHandlerPara->pucName, (INT8*)pucTmpGetParaPath);\n")
#                        f.write(' '*8 + "strcat((INT8*)pDmHandlerPara->pucName, \"%d.\");\n")
                        f.write(' '*8 + "sprintf((INT8*)pDmHandlerPara->pucName,\"%s%d.\",(INT8*)pDmHandlerPara->pucName, alIndex[i+1]);\n")
                    f.write(' '*8 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \"" + self.itemList[dbname][index].nameReplace + "\");\n")
                    if self.itemList[dbname][index].isObject:
                        f.write(' '*8 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \".\");\n")
                        f.write(' '*8 + "pDmHandlerPara->bItem = DM_FALSE;\n")
                    else:
                        f.write(' '*8 + "pDmHandlerPara->bItem = DM_TRUE;\n")
                    f.write(' '*8 + "pDmHandlerPara->ulParaId = " + self.itemList[dbname][index].definition + ";\n")
                    f.write(' '*8 + "pDmHandlerPara->pValue = (INT8 *)&(pDmHandlerValue[i]." + self.itemList[dbname][index].name + ");\n")
                    f.write(' '*8 + "pDmHandlerPara->valueSize = sizeof(pDmHandlerValue[i]." + self.itemList[dbname][index].name + ");\n")

                    f.write("\n")
                    f.write(' '*8 + "bRtn = DmCallDefaultGetHandler(pDmHandlerPara);\n")
                    f.write(' '*8 + "if (!bRtn)\n")
                    f.write(' '*8 + "{\n")
                    if item.isMultObj:
                        f.write(' '*12 + "DM_FREE(pucTmpGetParaPath);\n")
                    f.write(' '*12 + "DM_FREE(pDmHandlerPara);\n")
                    f.write(' '*12 + "return DM_FALSE;\n")
                    f.write(' '*8 + "}\n\n")

                    if self.itemList[dbname][index].cType[-2:] == "[]":
                        f.write(' '*8 + "pDmHandlerValue[i]."+self.itemList[dbname][index].name+"Cnt = pDmHandlerPara->ulCnt;\n")

                    if self.itemList[dbname][index].isMultObj:
                        f.write("\n")
                        f.write(' '*8 + "pDmHandlerValue[i]." + self.itemList[dbname][index].name + "_cnt = pDmHandlerPara->ulCnt;\n")
                        f.write("\n")


                    if item.isMultObj:
                        f.write(' '*8 + "memset(pDmHandlerPara->pucName, 0, 1024);\n")
                    else:
                        f.write(' '*8 + "memset(pDmHandlerPara->pucName, 0, 1024);\n")
                        f.write(' '*8 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pGetPara->pucPath);\n\n")

            if item.isMultObj:
                f.write("\n")
                f.write(' '*8 + "if(DM_FALSE == bInstance)\n")
                f.write(' '*8 + "{\n")
                f.write(' '*12 + "pDmHandlerPara->pDbHandler->xpathCtx->node = nodeBak;\n")
                f.write(' '*12 + "xmlXPathFreeObject(xpathObj);\n")
                f.write(' '*8 + "}\n")
            f.write(' '*4 + "}\n\n")
        else:
            f.write(' '*4 + "pDmHandlerPara->ulParaId = pGetPara->ulParaId;\n")
            f.write(' '*4 + "pDmHandlerPara->bItem = DM_TRUE;\n")
            f.write(' '*4 + "pDmHandlerPara->pValue = (INT8*)pGetPara->pValue;\n")
            f.write(' '*4 + "pDmHandlerPara->valueSize = pGetPara->ValueSize;\n\n")

            f.write(' '*4 + "bRtn = DmCallDefaultGetHandler(pDmHandlerPara);\n")
            f.write(' '*4 + "if (!bRtn)\n")
            f.write(' '*4 + "{\n")
            f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
            f.write(' '*8 + "return DM_FALSE;\n")
            f.write(' '*4 + "}\n\n")
            if item.cType[-2:] == "[]":
                f.write(' '*4 + "pGetPara->ulCnt = pDmHandlerPara->ulCnt;\n") # cyq:I think it's needed


        if item.isMultObj:
            f.write(' '*4 + "DM_FREE(pucTmpGetParaPath);\n")
        f.write(' '*4 + "DM_FREE(pDmHandlerPara->pucName);\n")
        f.write(' '*4 + "DM_FREE(pDmHandlerPara);\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\\n\");\n")

        f.write(' '*4 + "return DM_TRUE;\n")
        f.write("}\n\n")

    def genSetFunc(self,f,item,dbname):
        if item.isObject:
            funcName = "Set_" + item.nodeFullName.replace(".", "_") + "Req"
        else:
            funcName = "Set_" + item.nodeFullName.replace(".", "_") + "_Req"

        item.operationFunc["set"] = funcName

        f.write("INT32 "+ funcName + "(DM_SET_PARA_C_VALUE_REQ_T *pSetPara)\n")
        f.write("{\n")

        f.write(' '*4 + "DM_HANDLER_PARA_T  *pDmHandlerPara;\n")
        f.write(' '*4 + item.struction + "  *pDmHandlerValue;\n")
        if item.isObject:
            f.write(' '*4 + "UINT32              i;\n")
        f.write(' '*4 + "BOOL8              bRtn;\n\n")

        if item.isMultObj:
            f.write(' '*4 + "xmlXPathObjectPtr   xpathObj = NULL;\n")
            f.write(' '*4 + "xmlNodePtr          nodeBak = NULL;\n")
            f.write(' '*4 + "BOOL8               bInstance;\n")
            f.write(' '*4 + "INT8                acNodeName[16] = {0};\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\\n\");\n")

        f.write(' '*4 + "pDmHandlerPara = (DM_HANDLER_PARA_T *)malloc(sizeof(DM_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "if (pDmHandlerPara == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pDmHandlerPara\\n\");\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*8 + "}\n\n")

        f.write(' '*4 + "memset(pDmHandlerPara, 0, sizeof(DM_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "pDmHandlerPara->pDbHandler = GetDmHandlerByName(pSetPara->pucPath);\n")
        f.write(' '*4 + "if (pDmHandlerPara->pDbHandler == NULL)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ Para %s GetDmHandlerByName failed!\\n\", pSetPara->pucPath);\n")
        f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n")
        f.write(' '*4 + "pDmHandlerPara->TransactionId = pSetPara->TransactionId;\n")
        f.write(' '*4 + "pDmHandlerPara->bCInterface = DM_TRUE;\n")
        f.write(' '*4 + "pDmHandlerPara->ulCnt = 1;\n\n")

        f.write(' '*4 + "pDmHandlerPara->pucName = (INT8 *)malloc(1024);\n")
        f.write(' '*4 + "memset(pDmHandlerPara->pucName, 0, 1024);\n\n")
        if not item.isMultObj:
            f.write(' '*4 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pSetPara->pucPath);\n\n")

        f.write(' '*4 + "pDmHandlerValue = (" + item.struction + " *)pSetPara->pValue;\n\n")

        if item.isObject:
            #pasre the object
            f.write(' '*4 + "pDmHandlerPara->bItem = DM_FALSE;\n\n")
            if item.isMultObj:
                f.write(' '*4 + "INT8 *pucTmpParaPath = (INT8*)calloc(1, 1024);\n")

                f.write(' '*4 + "if (pucTmpParaPath == NULL)\n")
                f.write(' '*4 + "{\n")

                f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " calloc() failed for pucTmpParaPath\\n\");\n")
                f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
                f.write(' '*8 + "return DM_FALSE;\n")
                f.write(' '*8 + "}\n\n")

                f.write(' '*4 + "bInstance = DmIsSingleOfMultiple(pSetPara->pucPath);\n")
                f.write(' '*4 + "if(DM_TRUE == bInstance)\n")
                f.write(' '*4 + "{\n")
                f.write(' '*8 + "strncpy(pucTmpParaPath, pSetPara->pucPath, DmGetObjectLengthOfInstance(pSetPara->pucPath));\n")
                f.write(' '*4 + "}\n")
                f.write(' '*4 + "else\n")
                f.write(' '*4 + "{\n")
                f.write(' '*8 + "strcpy(pucTmpParaPath, pSetPara->pucPath);\n")
                f.write(' '*4 + "}\n")
            f.write(' '*4 + "for(i=0;i<pSetPara->ulCnt;i++)\n")
            f.write(' '*4 + "{\n")

            if item.isMultObj:
                f.write(' '*8 + "if(DM_FALSE == bInstance)\n")
                f.write(' '*8 + "{\n")
                f.write(' '*12 + "nodeBak = pDmHandlerPara->pDbHandler->xpathCtx->node;\n")
                f.write(' '*12 + "memset(acNodeName,0,sizeof(INT8)*16);\n")
                f.write(' '*12 + "sprintf(acNodeName,\"%s%d\",\"i\", pDmHandlerValue[i].instanceNum);\n")
                f.write(' '*12 + "xpathObj = DbGetXPathObjPtr(pDmHandlerPara->pDbHandler->xpathCtx, (INT8 *)acNodeName);\n")
                f.write(' '*8 + "}\n\n")

            for index in range(item.selfIndex+1, self.maxIndex[dbname]):
##                if self.itemList[index].operationFunc["set"] == "":
##                    f.write(' '*8 + "//!!! can't find the sub function of " + self.itemList[index].nodeFullName + " in set function.\n")
##                    continue
                if self.itemList[dbname][index].parentIndex == item.selfIndex:
                    if self.itemList[dbname][index].isMultObj:
                        f.write(' '*8 + "pDmHandlerPara->ulCnt = pDmHandlerValue[i]." + self.itemList[dbname][index].name + "_cnt;\n")
                        f.write(' '*8 + "if (pDmHandlerPara->ulCnt != 0)\n")
                        f.write(' '*8 + "{\n")

                        if item.isMultObj:
                            f.write(' '*12 + "strcat((INT8*)pDmHandlerPara->pucName, (INT8*)pucTmpParaPath);\n")
#                            f.write(' '*12 + "strcat((INT8*)pDmHandlerPara->pucName, \"%d.\");\n")
                            f.write(' '*12 + "sprintf((INT8*)pDmHandlerPara->pucName,\"%s%d.\",(INT8*)pDmHandlerPara->pucName, pDmHandlerValue[i].instanceNum);\n")
                        f.write(' '*12 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \"" + self.itemList[dbname][index].nameReplace + "\");\n")
                        if self.itemList[dbname][index].isObject:
                            f.write(' '*12 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \".\");\n")
                            f.write(' '*12 + "pDmHandlerPara->bItem = DM_FALSE;\n")
                        else:
                            f.write(' '*12 + "pDmHandlerPara->bItem = DM_TRUE;\n")
                        f.write(' '*12 + "pDmHandlerPara->ulParaId = " + self.itemList[dbname][index].definition + ";\n")
                        f.write(' '*12 + "pDmHandlerPara->pValue = (INT8 *)&(pDmHandlerValue[i]." + self.itemList[dbname][index].name + ");\n")
                        f.write(' '*12 + "pDmHandlerPara->valueSize = sizeof(pDmHandlerValue[i]." + self.itemList[dbname][index].name + ");\n")

                        f.write("\n")
                        f.write(' '*12 + "bRtn = DmCallDefaultSetHandler(pDmHandlerPara);\n")
                        f.write(' '*12 + "if (!bRtn)\n")
                        f.write(' '*12 + "{\n")
                        if item.isMultObj:
                            f.write(' '*16 + "DM_FREE(pucTmpParaPath);\n")
                        f.write(' '*16 + "DM_FREE(pDmHandlerPara);\n")
                        f.write(' '*16 + "return DM_FALSE;\n")
                        f.write(' '*12 + "}\n\n")

                        f.write(' '*12 + "memset(pDmHandlerPara->pucName, 0, 1024);\n")
                        if item.isMultObj:
                            pass
                        else:
                            f.write(' '*12 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pSetPara->pucPath);\n\n")

                        f.write(' '*8 + "}\n\n")
                    else:
                        #f.write(' '*8 + "pDmHandlerPara->ulCnt = 1;\n")
                        if item.isMultObj:
                            f.write(' '*8 + "strcat((INT8*)pDmHandlerPara->pucName, (INT8*)pucTmpParaPath);\n")
#                            f.write(' '*8 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \"%d.\");\n")
                            f.write(' '*8 + "sprintf((INT8*)pDmHandlerPara->pucName,\"%s%d.\",(INT8*)pDmHandlerPara->pucName, pDmHandlerValue[i].instanceNum);\n")
                        f.write(' '*8 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \"" + self.itemList[dbname][index].nameReplace + "\");\n")
                        if self.itemList[dbname][index].isObject:
                            f.write(' '*8 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \".\");\n")
                            f.write(' '*8 + "pDmHandlerPara->bItem = DM_FALSE;\n")
                        else:
                            f.write(' '*8 + "pDmHandlerPara->bItem = DM_TRUE;\n")
                        f.write(' '*8 + "pDmHandlerPara->ulParaId = " + self.itemList[dbname][index].definition + ";\n")
                        f.write(' '*8 + "pDmHandlerPara->pValue = (INT8 *)&(pDmHandlerValue[i]." + self.itemList[dbname][index].name + ");\n")
                        f.write(' '*8 + "pDmHandlerPara->valueSize = sizeof(pDmHandlerValue[i]." + self.itemList[dbname][index].name + ");\n")
                        if self.itemList[dbname][index].cType[-2:] == "[]":
                            f.write(' '*8 + "pDmHandlerPara->ulCnt = pDmHandlerValue[i]."+ self.itemList[dbname][index].name +"Cnt;\n")
                        else:
                            f.write(' '*8 + "pDmHandlerPara->ulCnt = 1;\n")

                        f.write("\n")
                        f.write(' '*8 + "bRtn = DmCallDefaultSetHandler(pDmHandlerPara);\n")
                        f.write(' '*8 + "if (!bRtn)\n")
                        f.write(' '*8 + "{\n")
                        if item.isMultObj:
                            f.write(' '*12 + "DM_FREE(pucTmpParaPath);\n")
                        f.write(' '*12 + "DM_FREE(pDmHandlerPara);\n")
                        f.write(' '*12 + "return DM_FALSE;\n")
                        f.write(' '*8 + "}\n\n")

                        if item.isMultObj:
                            f.write(' '*8 + "memset(pDmHandlerPara->pucName, 0, 1024);\n")
                        else:
                            f.write(' '*8 + "memset(pDmHandlerPara->pucName, 0, 1024);\n")
                            f.write(' '*8 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pSetPara->pucPath);\n\n")

            if item.isMultObj:
                f.write("\n")
                f.write(' '*8 + "if(DM_FALSE == bInstance)\n")
                f.write(' '*8 + "{\n")
                f.write(' '*12 + "pDmHandlerPara->pDbHandler->xpathCtx->node = nodeBak;\n")
                f.write(' '*12 + "xmlXPathFreeObject(xpathObj);\n")
                f.write(' '*8 + "}\n")

            f.write(' '*4 + "}\n\n")
        else:
            f.write(' '*4 + "pDmHandlerPara->ulParaId = pSetPara->ulParaId;\n")
            f.write(' '*4 + "pDmHandlerPara->bItem = DM_TRUE;\n")
            f.write(' '*4 + "pDmHandlerPara->pValue = (INT8*)pSetPara->pValue;\n")
            f.write(' '*4 + "pDmHandlerPara->valueSize = pSetPara->ValueSize;\n")
            f.write(' '*4 + "pDmHandlerPara->ulCnt = pSetPara->ulCnt;\n\n")

            f.write(' '*4 + "bRtn = DmCallDefaultSetHandler(pDmHandlerPara);\n")
            f.write(' '*4 + "if (!bRtn)\n")
            f.write(' '*4 + "{\n")
            f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
            f.write(' '*8 + "return DM_FALSE;\n")
            f.write(' '*4 + "}\n\n")

        if item.isMultObj:
            f.write(' '*4 + "DM_FREE(pucTmpParaPath);\n")
        f.write(' '*4 + "DM_FREE(pDmHandlerPara->pucName);\n")
        f.write(' '*4 + "DM_FREE(pDmHandlerPara);\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\\n\");\n")

        f.write(' '*4 + "return DM_TRUE;\n")
        f.write("}\n\n")

    def genDelFunc(self,f,item,dbname):
        funcName = "Del_" + item.nodeFullName.replace(".", "_") + "Req"

        item.operationFunc["del"] = funcName

        f.write("INT32 "+ funcName + "(DM_DELETE_OBJECT_REQ_T *pDelPara)\n")
        f.write("{\n")

        f.write(' '*4 + "DM_HANDLER_PARA_T  *pDmHandlerPara;\n")
        f.write(' '*4 + "BOOL8              bRtn;\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ enter " + funcName + "\\n\");\n")

        f.write(' '*4 + "pDmHandlerPara = (DM_HANDLER_PARA_T *)malloc(sizeof(DM_HANDLER_PARA_T));\n\n")

        f.write(' '*4 + "if (pDmHandlerPara == NULL)\n")
        f.write(' '*4 + "{\n")

        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ " + funcName + " malloc() failed for pDmHandlerPara\\n\");\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*8 + "}\n\n")

        f.write(' '*4 + "memset(pDmHandlerPara, 0, sizeof(DM_HANDLER_PARA_T));\n\n")
        f.write(' '*4 + "pDmHandlerPara->pDbHandler = GetDmHandlerByName(pDelPara->pucObjName);\n")
        f.write(' '*4 + "if (pDmHandlerPara->pDbHandler == NULL)\n")
        f.write(' '*4 + "{\n")
        f.write(' '*8 + "DM_TRACE(DM_TRACE_LEVEL_HIGH, \"DM Trace ------ Para %s GetDmHandlerByName failed!\\n\", pDelPara->pucObjName);\n")
        f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
        f.write(' '*8 + "return DM_FALSE;\n")
        f.write(' '*4 + "}\n\n")

        f.write(' '*4 + "pDmHandlerPara->TransactionId = pDelPara->TransactionId;\n")
        f.write(' '*4 + "pDmHandlerPara->ulParaId = " + item.definition + ";\n")
        f.write(' '*4 + "pDmHandlerPara->bCInterface = DM_TRUE;\n")
        f.write(' '*4 + "pDmHandlerPara->ulCnt = 1;\n\n")

        f.write(' '*4 + "pDmHandlerPara->pucName = (INT8 *)malloc(1024);\n")
        f.write(' '*4 + "memset(pDmHandlerPara->pucName, 0, 1024);\n\n")
        f.write(' '*4 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pDelPara->pucObjName);\n\n")

        f.write(' '*4 + "pDmHandlerPara->pValue = NULL;\n")
        f.write(' '*4 + "pDmHandlerPara->valueSize = 0;\n\n")

        for index in range(item.selfIndex+1, self.maxIndex[dbname]):
##            if self.itemList[index].operationFunc["del"] == "":
##                f.write(' '*4 + "//!!! can't find the sub function of " + self.itemList[index].nodeFullName + " in del function.\n")
##                continue
            if self.itemList[dbname][index].parentIndex == item.selfIndex:
                f.write(' '*4 + "pDmHandlerPara->ulParaId = " + self.itemList[dbname][index].definition + ";\n")
                f.write(' '*4 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \"" + self.itemList[dbname][index].nameReplace + "\");\n")
                if self.itemList[dbname][index].isObject:
                    f.write(' '*4 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \".\");\n")
                    if self.itemList[dbname][index].isMultObj:
                        f.write(' '*4 + "pDmHandlerPara->pucName = (INT8*)strcat((INT8*)pDmHandlerPara->pucName, \"{i}.\");\n")
                    f.write(' '*4 + "pDmHandlerPara->bItem = DM_FALSE;\n")
                else:
                    f.write(' '*4 + "pDmHandlerPara->bItem = DM_TRUE;\n")

                f.write("\n")

                f.write(' '*4 + "bRtn = DmCallDefaultDelHandler(pDmHandlerPara);\n")
                f.write(' '*4 + "if (!bRtn)\n")
                f.write(' '*4 + "{\n")
                f.write(' '*8 + "DM_FREE(pDmHandlerPara);\n")
                f.write(' '*8 + "return DM_FALSE;\n")
                f.write(' '*4 + "}\n\n")

                f.write(' '*4 + "memset(pDmHandlerPara->pucName, 0, 1024);\n")
                f.write(' '*4 + "strcpy((INT8*)pDmHandlerPara->pucName, (INT8*)pDelPara->pucObjName);\n\n")

        f.write(' '*4 + "DM_FREE(pDmHandlerPara->pucName);\n")
        f.write(' '*4 +"DM_FREE(pDmHandlerPara);\n\n")

        f.write(' '*4 + "DM_TRACE(DM_TRACE_LEVEL_LOW, \"DM Trace ------ leave " + funcName + "\\n\");\n")

        f.write(' '*4 +"return DM_TRUE;\n")
        f.write("}\n\n")

    def genOperationFile(self, filePath, dbname, fileNamePrefix, fileNameSuffix, incFileName):
        """
        generate the Operation file
        """
        if not os.path.exists(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/"):
            os.makedirs(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/")

        logger.debug('writing '+ResultSrcDir + "/db_" + str(dbname).lower() + "/src/" + fileNamePrefix+"convert"+fileNameSuffix)
        convf=open(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/" + fileNamePrefix+"convert"+fileNameSuffix,  'wb')
        #addf=open(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/" + fileNamePrefix+"add"+fileNameSuffix,  'wb')
        #delf=open(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/" + fileNamePrefix+"del"+fileNameSuffix,  'wb')
        #getf=open(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/" + fileNamePrefix+"get"+fileNameSuffix,  'wb')
        #setf=open(ResultSrcDir + "/db_" + str(dbname).lower() + "/src/" + fileNamePrefix+"set"+fileNameSuffix,  'wb')

        #write the include
        convf.write('#include "dm_internal.h"\n')
        convf.write('#include \"'+ incFileName + '\"\n\n')

        #addf.write('#include "dm_internal.h"\n')
        #addf.write('#include \"'+ incFileName + '\"\n\n')

        #delf.write('#include "dm_internal.h"\n')
        #delf.write('#include \"'+ incFileName + '\"\n\n')

        #getf.write('#include "dm_internal.h"\n')
        #getf.write('#include \"'+ incFileName + '\"\n\n')

        #setf.write('#include "dm_internal.h"\n')
        #setf.write('#include \"'+ incFileName + '\"\n\n')

        convf.write('/*----------beginning of convert function list----------*/\n\n')
        #gen the convert func for BITMAP and Enum
        for tmpItem in self.itemList[dbname]:
            if tmpItem.cType == "UINT32_BITMAP":
                self.convertBitmapToString(convf,tmpItem)
                self.convertStringToBitmap(convf,tmpItem)
            if tmpItem.cType == "UINT32_ENUM":
                self.convertEnumToString(convf,tmpItem)
                self.convertStringToEnum(convf,tmpItem)
        convf.write('/*----------end of convert function list----------*/\n\n')

        convf.close()

        #get Func Generation of each item
        #tmpFuncGen = 0
        #for tmpItem in self.itemList[dbname][::-1]:
        #    if re.search('A', tmpItem.genOperationFlg, re.IGNORECASE):          #add operation
        #        if tmpItem.isObject:
        #            self.genAddFunc(addf,tmpItem, dbname)
        #    if re.search('G', tmpItem.genOperationFlg, re.IGNORECASE):          #get operation
        #        self.genGetFunc(getf,tmpItem, dbname)
        #    if re.search('S', tmpItem.genOperationFlg, re.IGNORECASE):          #set operation
        #        self.genSetFunc(setf,tmpItem, dbname)
        #    if re.search('D', tmpItem.genOperationFlg, re.IGNORECASE):          #delete operation
        #        if tmpItem.isObject:
        #            self.genDelFunc(delf,tmpItem, dbname)

        #addf.close()
        #delf.close()
        #getf.close()
        #setf.close()

###end of Operation Files

    def genXmlFile(self, filePath, fileName):
        pass

    def genMappingXmlObject(self, index, parentNode, dom, dbname):
        """
        generate objects in Mapping.xml file
        """

        #print "Object : "+self.itemList[index].nodeFullNameReplace

        if self.itemList[dbname][index].nodeFullNameReplace.endswith('{i}.'):
            rebuildFullName = self.itemList[dbname][index].nodeFullNameReplace.replace('{i}.','')

            configname="configDataFile.configData."+rebuildFullName.replace(self.rootName[dbname],'')
            dataname=rebuildFullName

        else :
            configname="configDataFile.configData."+self.itemList[dbname][index].nodeFullNameReplace.replace(self.rootName[dbname],'')
            dataname=self.itemList[dbname][index].nodeFullNameReplace

        paramNode = dom.createElement('Mapping')
        paramNode.setAttribute("configname", configname[:-1])
        paramNode.setAttribute("dataname", dataname)
        paramNode.setAttribute("type", "ObjectChange")

        keyParams=""
        saveParams = ""

        childPrefix = ""

        runningflg = 1
        childIndex = index+1
        while runningflg==1:
            #check if out of range
            if index >= self.maxIndex[dbname] or childIndex >= self.maxIndex[dbname]:
                runningflg=0
                continue

            #check if to the end of current object
            if self.itemList[dbname][childIndex].nodeFullNameReplace.find(self.itemList[dbname][index].nodeFullNameReplace)==-1:
                runningflg=0
                continue
            else:
                childPrefix = self.itemList[dbname][childIndex].nodeFullNameReplace.replace(self.itemList[dbname][index].nodeFullNameReplace,'')

            if self.itemList[dbname][childIndex].AsKeyParam==1:
                keyParams += self.itemList[dbname][childIndex].nameReplace+","

            #check if necessary to write this parameter/object
            if self.itemList[dbname][childIndex].AsVendorConfig!=1 and self.itemList[dbname][childIndex].AsKeyParam!=1:
                childIndex += 1
                continue

            #insert xml node
            if self.itemList[dbname][childIndex].isObject:
                childIndex = self.genMappingXmlInnerObject(childIndex, childPrefix, paramNode, dom, dbname)
            else:
                childIndex = self.genMappingXmlInnerParameter(childIndex, childPrefix, paramNode, dom, dbname)
                saveParams += self.itemList[dbname][childIndex].nameReplace+","

        if len(keyParams)>0:
            keyParams = keyParams[:-1]
        elif len(saveParams)>0:
            keyParams = saveParams[:-1]

        paramNode.setAttribute("key", keyParams)

        parentNode.appendChild(paramNode)

        return childIndex

    def genMappingXmlInnerObject(self, index, prefix, parentNode, dom, dbname):
        """
        generate inner objects in Mapping.xml file
        """
        #print "InnerObject : prefix="+prefix+" : "+self.itemList[index].nodeFullNameReplace

        #prefix should be like 'a.b.c.{i}.'
        myPrefix = prefix.replace("{i}.",'')
        paramNode = dom.createElement(myPrefix[:-1])

        keyParams = ""
        saveParams = ""

        runningflg = 1
        childIndex = index+1
        childPrefix = ""
        while runningflg==1:
            #check if out of range
            if index >= self.maxIndex[dbname] or childIndex >= self.maxIndex[dbname]:
                runningflg=0
                continue

            #check if to the end of current object
            #logger.debug('genMappingXmlInnerObject index = '+str(index)+' childIndex = '+str(childIndex)+' '+self.itemList[childIndex].nodeFullNameReplace)
            if self.itemList[dbname][childIndex].nodeFullNameReplace.find(self.itemList[dbname][index].nodeFullNameReplace)==-1:
                runningflg=0
                continue
            else:
                childPrefix = self.itemList[dbname][childIndex].nodeFullNameReplace.replace(self.itemList[dbname][index].nodeFullNameReplace,'')

            if self.itemList[dbname][childIndex].AsKeyParam==1:
                keyParams += self.itemList[dbname][childIndex].nameReplace+","

            #check if necessary to write this parameter/object
            if self.itemList[dbname][childIndex].AsVendorConfig!=1 and self.itemList[dbname][childIndex].AsKeyParam!=1:
                childIndex += 1
                continue

            #insert xml node
            if self.itemList[dbname][childIndex].isObject:
                childIndex = self.genMappingXmlInnerObject(childIndex, childPrefix, paramNode, dom, dbname)
            else:
                childIndex = self.genMappingXmlInnerParameter(childIndex, childPrefix, paramNode, dom, dbname)
                saveParams += self.itemList[dbname][childIndex].nameReplace+","

        if len(keyParams)>0:
            keyParams = keyParams[:-1]
        elif len(saveParams)>0:
            keyParams = saveParams[:-1]

        paramNode.setAttribute("type", "ObjectChange")
        paramNode.setAttribute("key", keyParams)
        parentNode.appendChild(paramNode)

        return childIndex

    def genMappingXmlInnerParameter(self, index, prefix, parentNode, dom, dbname):
        """
        generate inner parameters in Mapping.xml file
        """

        paramNode = dom.createElement(prefix)

        paramType=""
        if self.itemList[dbname][index].AsVendorConfig==1:
            paramType="ValueChange"
        elif self.itemList[dbname][index].AsKeyParam==1:
            paramType="ValueKeep"

        paramNode.setAttribute("type", paramType)
        parentNode.appendChild(paramNode)

        return index+1

    def genMappingXmlParameter(self, index, parentNode, dom, dbname):
        """
        generate parameters in Mapping.xml file
        """

        configname="configDataFile.configData."+self.itemList[dbname][index].nodeFullNameReplace.replace(self.rootName[dbname],'')
        dataname=self.itemList[dbname][index].nodeFullNameReplace

        paramNode = dom.createElement('Mapping')
        paramNode.setAttribute("configname", configname)
        paramNode.setAttribute("dataname", dataname)
        paramNode.setAttribute("type", "ValueChange")

        parentNode.appendChild(paramNode)

        return index+1

    def genMappingXmlFile(self, filePath, dbname, fileName):
        """
        generate Mapping.xml file for vendor config file
        """

        import xml.dom.minidom
        impl = xml.dom.minidom.getDOMImplementation()
        dom = impl.createDocument(None, 'MappingTable', None)
        root = dom.documentElement

        index = 0
        while index<self.maxIndex[dbname]:
            if self.itemList[dbname][index].AsVendorConfig==1:
                if self.itemList[dbname][index].isObject:
                    index = self.genMappingXmlObject(index, root, dom, dbname)
                else:
                    index = self.genMappingXmlParameter(index, root, dom, dbname)
            else:
                index += 1

        if not os.path.exists(ResultSrcDir + "/db_" + str(dbname).lower() + "/dst/"):
            os.makedirs(ResultSrcDir + "/db_" + str(dbname).lower() + "/dst/")

        f = open(ResultSrcDir + "/db_" + str(dbname).lower() + "/dst/" + fileName,  'wb')
        dom.writexml(f, addindent='  ', newl='\n',encoding='utf-8')
        f.close()

    def genDbCbMakeFile(self,dbname,subMod,libExtension):
        dbCbResultSrcDir = ResultSrcDir + "/db_cbs/"+str(subMod).lower()+"/"
        if not os.path.exists(dbCbResultSrcDir):
            return
        if os.path.exists(dbCbResultSrcDir+"/Makefile"):
            return
        logger.debug("writing "+ dbCbResultSrcDir +"/Makefile")

        f = open(dbCbResultSrcDir + "/Makefile","wb")

        f.write("#auto-generated by python script.\n\n")

        f.write("ifndef PRC_CROSS_COMPILE\n")
        if str(dbname).lower()=="cndata":
            f.write("PRC_CROSS_COMPILE := \n")
        else:
            f.write("PRC_CROSS_COMPILE := /opt/buildroot-gcc463/usr/bin/mipsel-linux-\n")
        f.write("endif\n")

        f.write("CC := $(PRC_CROSS_COMPILE)gcc\n")
        f.write("AR := $(PRC_CROSS_COMPILE)ar\n")
        f.write("link := $(PRC_CROSS_COMPILE)gcc\n")
        f.write("STRIP := $(PRC_CROSS_COMPILE)strip\n")

        f.write("\n"*2)

        f.write("COM_LIB_PATH := " + calcRelativePath(dbCbResultSrcDir, CommonSrcDir) + "\n")
        f.write("DM_INC_PATH = $(COM_LIB_PATH)/dm_lib/common/inc\n")
        f.write("DB_EXT_INC_PATH = $(COM_LIB_PATH)/dm_lib/external_type\n")
        f.write("LIBXML2_INC_PATH = $(COM_LIB_PATH)/dm_lib/libxml2/include\n")

        f.write("RESULT_PATH := " + calcRelativePath(dbCbResultSrcDir, ResultDbLibDir) + "\n")
        f.write("OBJ_PATH := ../../obj\n")
        f.write("OBJ_PATH_DMALLOC := ../../obj_dmalloc\n")

        f.write("\n"*2)
        #f.write("DB_INC_PATH = ../../inc\n")
        f.write("DB_CBS_SRC_PATH := ./\n")

        f.write("\n"*2)
        f.write("OSA_INCLUDES = $(COM_LIB_PATH)/osa/inc\n")
        f.write("COM_INCLUDES = $(COM_LIB_PATH)/include\n")
        f.write("DMALLOC_INCLUDES = $(COM_LIB_PATH)/dmalloc\n")

        f.write("\n"*2)
        f.write("DB_CBS_INCLUDES := -I$(DB_EXT_INC_PATH) -I$(DM_INC_PATH) -I$(COM_INCLUDES) -I$(OSA_INCLUDES) -I$(LIBXML2_INC_PATH) -I$(DMALLOC_INCLUDES)\n")
        f.write("DB_CBS_OBJ_CFLAGS := -g -Wall -Wshadow -Wcast-qual\n")

        if libExtension.lower()=="all" or libExtension.lower()=="so":
            f.write("\n"*2)
            f.write("DB_CBS_OBJ_CFLAGS += -fPIC\n")
            f.write("DB_CBS_OBJ_CFLAGS_DMALLOC += $(DB_CBS_OBJ_CFLAGS) -DDMALLOC_CHECK_ENABLED\n")
            f.write("DB_CBS_LIB_CFLAGS := -g -Wall -Wshadow -Wcast-qual -O -shared -o\n")
        else:
            pass

        f.write("\n"*2)
        if libExtension.lower()=="all" or libExtension.lower()=="so":
            f.write("DB_CBS_LIB = $(RESULT_PATH)/lib"+str(subMod).capitalize()+"DbCbs.so\n")
            f.write("DB_CBS_LIB_DMALLOC = $(RESULT_PATH)/lib"+str(subMod).capitalize()+"DbCbs_dmalloc.so\n")
        else:
            pass

        if libExtension.lower()=="all" or libExtension.lower()=="a":
            f.write("DB_CBS_STATIC_LIB = $(RESULT_PATH)/lib"+str(subMod).capitalize()+"DbCbs.a\n")
            f.write("DB_CBS_STATIC_LIB_DMALLOC = $(RESULT_PATH)/lib"+str(subMod).capitalize()+"DbCbs_dmalloc.a\n")
        else:
            pass

        f.write("DB_CBS_SRC := $(wildcard $(DB_CBS_SRC_PATH)/*.c)\n")
        f.write("DB_CBS_OBJ := $(patsubst $(DB_CBS_SRC_PATH)/%,$(OBJ_PATH)/%,$(patsubst %.c,%.o,$(DB_CBS_SRC)))\n")
        f.write("DB_CBS_OBJ_DMALLOC := $(patsubst $(DB_CBS_SRC_PATH)/%,$(OBJ_PATH_DMALLOC)/%,$(patsubst %.c,%.o,$(DB_CBS_SRC)))\n")

        f.write("\n"*2)
        if libExtension.lower()=="all":
            f.write("ifdef BRCM_DMALLOC_ENABLED\n")
            f.write("all : $(DB_CBS_STATIC_LIB) $(DB_CBS_LIB) $(DB_CBS_STATIC_LIB_DMALLOC) $(DB_CBS_LIB_DMALLOC)\n")
            f.write("else\n")
            f.write("all : $(DB_CBS_STATIC_LIB) $(DB_CBS_LIB)\n")
            f.write("endif\n\n")
        else:
            pass

        if libExtension.lower()=="all" or libExtension.lower()=="a":
            f.write("$(DB_CBS_STATIC_LIB) : $(DB_CBS_OBJ)\n")
            f.write("\t"+"$(AR) -r $(DB_CBS_STATIC_LIB) $(DB_CBS_OBJ)\n\n")
            f.write("$(DB_CBS_STATIC_LIB_DMALLOC) : $(DB_CBS_OBJ_DMALLOC)\n")
            f.write("\t"+"$(AR) -r $(DB_CBS_STATIC_LIB_DMALLOC) $(DB_CBS_OBJ_DMALLOC)\n\n")
        else:
            pass

        if libExtension.lower()=="all" or libExtension.lower()=="so":
            f.write("$(DB_CBS_LIB) : $(DB_CBS_OBJ)\n")
            f.write("\t"+"$(CC) $(DB_CBS_LIB_CFLAGS) $(DB_CBS_LIB) $(DB_CBS_OBJ)\n\n")
            f.write("$(DB_CBS_LIB_DMALLOC) : $(DB_CBS_OBJ_DMALLOC)\n")
            f.write("\t"+"$(CC) $(DB_CBS_LIB_CFLAGS) $(DB_CBS_LIB_DMALLOC) $(DB_CBS_OBJ_DMALLOC)\n\n")
        else:
            pass

        f.write("$(DB_CBS_OBJ):$(OBJ_PATH)/%.o:$(DB_CBS_SRC_PATH)/%.c\n")
        f.write("\t"+"@mkdir -p $(OBJ_PATH)\n")
        f.write("\t"+"$(CC) $(DB_CBS_OBJ_CFLAGS) $(DB_CBS_INCLUDES) -pthread -c $< -o $@\n\n")

        f.write("$(DB_CBS_OBJ_DMALLOC):$(OBJ_PATH_DMALLOC)/%.o:$(DB_CBS_SRC_PATH)/%.c\n")
        f.write("\t"+"@mkdir -p $(OBJ_PATH_DMALLOC)\n")
        f.write("\t"+"$(CC) $(DB_CBS_OBJ_CFLAGS_DMALLOC) $(DB_CBS_INCLUDES) -pthread -c $< -o $@\n")

        f.write("\n"*2)
        f.write(".PHONY:clean\n")
        f.write("clean:\n")

        if libExtension.lower()=="all" or libExtension.lower()=="so":
            f.write("\t"+"$(RM) $(DB_CBS_LIB) $(DB_CBS_LIB_DMALLOC)\n")
        else:
            pass

        if libExtension.lower()=="all" or libExtension.lower()=="a":
            f.write("\t"+"$(RM) $(DB_CBS_STATIC_LIB) $(DB_CBS_STATIC_LIB_DMALLOC)\n")
        else:
            pass

        f.write("\t"+"$(RM) $(DB_CBS_OBJ) $(DB_CBS_OBJ_DMALLOC)\n")

        f.write('\n'*2)
        f.write(".PHONY:code_clean\n")
        f.write("code_clean:\n")
        f.write("\t"+"@$(RM) -rf ./*.c\n")
        f.write("\t"+"@$(RM) -rf $(DB_CBS_OBJ)\n")

        f.close()

    def genDbMakeFile(self, filepath, dbname, libExtension):
        dbFiles = ["_model_pattern"]
        dbResultSrcDir = ResultSrcDir + "/db_" + str(dbname).lower() + "/"
        if not os.path.exists(dbResultSrcDir):
            os.makedirs(dbResultSrcDir)

        logger.debug("writing "+ResultSrcDir + "/db_" + str(dbname).lower() + "/Makefile")

        f = open(ResultSrcDir + "/db_" + str(dbname).lower() + "/Makefile","wb")

        f.write("#auto-generated by python script.\n\n")


        f.write("ifndef PRC_CROSS_COMPILE\n")
        if str(dbname).lower()=="cndata":
            f.write("PRC_CROSS_COMPILE := \n")
        else:
            f.write("PRC_CROSS_COMPILE := /opt/buildroot-gcc463/usr/bin/mipsel-linux-\n")
        f.write("endif\n")
        f.write("CC := $(PRC_CROSS_COMPILE)gcc\n")
        f.write("AR := $(PRC_CROSS_COMPILE)ar\n")
        f.write("link := $(PRC_CROSS_COMPILE)gcc\n")
        f.write("STRIP := $(PRC_CROSS_COMPILE)strip\n")

        f.write("\n"*2)
        f.write("COM_LIB_PATH := " + calcRelativePath(dbResultSrcDir, CommonSrcDir) + "\n")
        f.write("DM_INC_PATH = $(COM_LIB_PATH)/dm_lib/common/inc\n")
        f.write("DB_EXT_INC_PATH = $(COM_LIB_PATH)/dm_lib/external_type\n")
        f.write("LIBXML2_INC_PATH = $(COM_LIB_PATH)/dm_lib/libxml2/include\n")

        f.write("\n"*2)
        f.write("DB_INC_PATH = ./inc\n")
        f.write("DB_SRCS_PATH = ./src\n")
        f.write("OBJ_PATH := ./obj\n")
        f.write("OBJ_PATH_DMALLOC := ./obj_dmalloc\n")
        f.write("RESULT_PATH := " + calcRelativePath(dbResultSrcDir, ResultDbLibDir) + "\n")

        f.write("\n"*2)
        f.write("OSA_INCLUDES = $(COM_LIB_PATH)/osa/inc\n")
        f.write("COM_INCLUDES = $(COM_LIB_PATH)/include\n")
        f.write("DMALLOC_INCLUDES = $(COM_LIB_PATH)/dmalloc\n")

        f.write("\n"*2)
        f.write("DB_INCLUDES := -I$(DB_INC_PATH) -I$(DB_EXT_INC_PATH) -I$(DM_INC_PATH) -I$(COM_INCLUDES) -I$(OSA_INCLUDES) -I$(LIBXML2_INC_PATH) -I$(DMALLOC_INCLUDES)\n")
        f.write("DB_OBJ_CFLAGS := -g -Wall -Wshadow -Wcast-qual\n")

        f.write("\n"*2)
        if libExtension.lower()=="all" or libExtension.lower()=="so":
            f.write("DB_LIB = $(RESULT_PATH)/lib"+str(dbname).capitalize()+"Db.so\n")
            f.write("DB_LIB_DMALLOC = $(RESULT_PATH)/lib"+str(dbname).capitalize()+"Db_dmalloc.so\n")

        if libExtension.lower()=="all" or libExtension.lower()=="a":
            f.write("DB_STATIC_LIB = $(RESULT_PATH)/lib"+str(dbname).capitalize()+"Db.a\n")
            f.write("DB_STATIC_LIB_DMALLOC = $(RESULT_PATH)/lib"+str(dbname).capitalize()+"Db_dmalloc.a\n")

        f.write("DB_OBJS := $(OBJ_PATH)/"+str(dbname).lower()+dbFiles[0]+".o\n")
        f.write("DB_OBJS_DMALLOC := $(OBJ_PATH_DMALLOC)/"+str(dbname).lower()+dbFiles[0]+".o\n")
        for dbFile in dbFiles[1:]:
            f.write("DB_OBJS += $(OBJ_PATH)/"+str(dbname).lower()+dbFile+".o\n")
            f.write("DB_OBJS_DMALLOC += $(OBJ_PATH_DMALLOC)/"+str(dbname).lower()+dbFile+".o\n")

        f.write("\n"*2)

        if libExtension.lower()=="all" or libExtension.lower()=="so":
            f.write("DB_OBJ_CFLAGS += -fPIC\n")
            f.write("DB_OBJ_CFLAGS_DMALLOC := $(DB_OBJ_CFLAGS) -DDMALLOC_CHECK_ENABLED\n")
            f.write("DB_LIB_CFLAGS := -g -Wall -Wshadow -Wcast-qual -O -shared -o\n")
            f.write("\n"*2)

        if libExtension.lower()=="all":
            f.write("ifdef BRCM_DMALLOC_ENABLED\n")
            f.write("all : $(DB_LIB) $(DB_STATIC_LIB) $(DB_LIB_DMALLOC) $(DB_STATIC_LIB_DMALLOC)\n")
            f.write("else\n")
            f.write("all : $(DB_LIB) $(DB_STATIC_LIB)\n")
            f.write("endif\n\n")

        if libExtension.lower()=="all" or libExtension.lower()=="a":
            f.write("$(DB_STATIC_LIB) : $(DB_OBJS)\n")
            f.write("\t"+"$(AR) -r $(DB_STATIC_LIB) $(DB_OBJS)\n\n")
            f.write("$(DB_STATIC_LIB_DMALLOC) : $(DB_OBJS_DMALLOC)\n")
            f.write("\t"+"$(AR) -r $(DB_STATIC_LIB_DMALLOC) $(DB_OBJS_DMALLOC)\n\n")

        if libExtension.lower()=="all" or libExtension.lower()=="so":
            f.write("$(DB_LIB) : $(DB_OBJS)\n")
            f.write("\t"+"$(CC) $(DB_LIB_CFLAGS) $(DB_LIB) $(DB_OBJS)\n\n")
            f.write("$(DB_LIB_DMALLOC) : $(DB_OBJS_DMALLOC)\n")
            f.write("\t"+"$(CC) $(DB_LIB_CFLAGS) $(DB_LIB_DMALLOC) $(DB_OBJS_DMALLOC)\n\n")

        for dbFile in dbFiles:
            f.write("$(OBJ_PATH)/"+str(dbname).lower()+dbFile+".o :./src/"+str(dbname).lower()+dbFile+".c\n")
            f.write("\t"+"@mkdir -p $(OBJ_PATH)\n")
            f.write("\t"+"$(CC) $(DB_OBJ_CFLAGS) $(DB_INCLUDES) -pthread -c $< -o $@\n\n")
            f.write("$(OBJ_PATH_DMALLOC)/"+str(dbname).lower()+dbFile+".o :./src/"+str(dbname).lower()+dbFile+".c\n")
            f.write("\t"+"@mkdir -p $(OBJ_PATH_DMALLOC)\n")
            f.write("\t"+"$(CC) $(DB_OBJ_CFLAGS_DMALLOC) $(DB_INCLUDES) -pthread -c $< -o $@\n\n")

        f.write('\n'*2)
        f.write(".PHONY:clean\n")
        f.write("clean:\n")

        if libExtension.lower()=="all" or libExtension.lower()=="so":
            f.write("\t"+"$(RM) $(DB_LIB) $(DB_LIB_DMALLOC)\n")
        else:
            pass

        if libExtension.lower()=="all" or libExtension.lower()=="a":
            f.write("\t"+"$(RM) $(DB_STATIC_LIB) $(DB_STATIC_LIB_DMALLOC)\n")
        else:
            pass

        f.write("\t"+"$(RM) $(DB_OBJS) $(DB_OBJS_DMALLOC)\n")

        f.write('\n'*2)
        f.write(".PHONY:code_clean\n")
        f.write("code_clean:\n")
        f.write("\t"+"$(RM) -rf ./src/*\n")
        f.write("\t"+"$(RM) -rf ../obj/*\n")

        f.close()

    def genDbDefaultXmlFile(self, targetPath, dbname):
        if not self.extDb[dbname] == 'DM_FALSE':
            return

        logger.debug("generating "+str(dbname).capitalize()+".xml")

        impl = xml.dom.minidom.getDOMImplementation()

        dom = impl.createDocument(None, str(dbname).capitalize(), None)
        root = dom.documentElement

        cmmt = dom.createComment('Description : '+self.itemList[dbname][0].description)
        dom.insertBefore(cmmt,root)

        objXmlMap = {}
        rootList = []
        rootIndex = 0

        rootList.append(('0', root))
        objXmlMap[str(rootIndex)] = rootList

        for item in self.itemList[dbname][1:]:
            if objXmlMap.has_key(str(item.parentIndex)):
                parentXmlNodeList = objXmlMap[str(item.parentIndex)]

                if item.isMultObj:
                    #logger.debug("handle multi-obj "+item.nodeFullNameReplace)

                    tmpItemXmlList = []
                    for parentInstanceNumList, parentXmlNode in parentXmlNodeList:
                        maxInstanceNum=0
                        itemDftValue = ""

                        itemDftValue = self.cPtnDftValueGen(item)[1:-1]

                        if itemDftValue.strip() == "":
                            continue

                        if itemDftValue.strip().startswith('['):
                            theInstList = parentInstanceNumList.strip().split(',')

                            theInstArray = ''
                            for instNum in theInstList:
                                if instNum != '0':
                                    if theInstArray == '':
                                        theInstArray = instNum
                                    else:
                                        theInstArray = theInstArray+','+instNum
                                else:
                                    pass

                            if theInstArray == '':
                                #no instance found
                                continue
                            else:
                                theInstArray = theInstArray+']='

                            itemDftValueList = itemDftValue.split('[')

                            for oneDftValue in itemDftValueList:
                                if oneDftValue.startswith(theInstArray):
                                    theDftValue = oneDftValue[len(theInstArray):]

                                    if len(theDftValue) > 0:
                                        maxInstanceNum = int(theDftValue)
                                    break
                                else:
                                    pass
                        else:
                            if not itemDftValue.strip() == "":
                                maxInstanceNum = int(itemDftValue.strip())

                        if maxInstanceNum>0:
                            if XmlWithComment.lower() == "yes" :
                                itemCmmt = dom.createComment('Description : '+item.description.strip().replace('\\n','\n').replace('\\"','"').replace('\r',''))
                                parentXmlNode.appendChild(itemCmmt)
                                itemCmmt = dom.createComment('Data Type : '+item.type+' ('+item.cType+')')
                                parentXmlNode.appendChild(itemCmmt)
                                itemCmmt = dom.createComment('Value Range : '+str(item.valueRange).strip().replace('\\n',',').replace('\n',',').replace('\r',''))
                                parentXmlNode.appendChild(itemCmmt)

                            paramXmlNode = dom.createElement(item.nameReplace.strip())
                            parentXmlNode.appendChild(paramXmlNode)

                            newInstanceNum = 1
                            while newInstanceNum<=maxInstanceNum:
                                instanceXmlNode = dom.createElement('i'+str(newInstanceNum))
                                paramXmlNode.appendChild(instanceXmlNode)

                                tmpItemXmlList.append((parentInstanceNumList+','+str(newInstanceNum),instanceXmlNode))
                                newInstanceNum = newInstanceNum+1
                        else:
                            #no need to create this object
                            continue

                    objXmlMap[str(item.selfIndex)]=tmpItemXmlList

                elif item.isObject:
                    #logger.debug("handle fix-obj "+item.nodeFullNameReplace)

                    tmpItemXmlList = []
                    for parentInstanceNumList, parentXmlNode in parentXmlNodeList:
                        if XmlWithComment.lower() == "yes" :
                            itemCmmt = dom.createComment('Description : '+item.description.replace('\\n','\n').replace('\\"','"').replace('\r',''))
                            parentXmlNode.appendChild(itemCmmt)
                            itemCmmt = dom.createComment('Data Type : '+item.type+' ('+item.cType+')')
                            parentXmlNode.appendChild(itemCmmt)
                            itemCmmt = dom.createComment('Value Range : '+str(item.valueRange).strip().replace('\\n',',').replace('\n',',').replace('\r',''))
                            parentXmlNode.appendChild(itemCmmt)

                        paramXmlNode = dom.createElement(item.nameReplace.strip())
                        parentXmlNode.appendChild(paramXmlNode)

                        tmpItemXmlList.append((parentInstanceNumList+',0',paramXmlNode))

                    objXmlMap[str(item.selfIndex)]=tmpItemXmlList

                else:
                    #logger.debug("handle leaf-node "+item.nodeFullNameReplace)

                    for parentInstanceNumList, parentXmlNode in parentXmlNodeList:
                        if XmlWithComment.lower() == "yes" :
                            itemCmmt = dom.createComment('Description : '+item.description.replace('\\n','\n').replace('\\"','"').replace('\r',''))
                            parentXmlNode.appendChild(itemCmmt)
                            itemCmmt = dom.createComment('Data Type : '+item.type+' ('+item.cType+')')
                            parentXmlNode.appendChild(itemCmmt)
                            itemCmmt = dom.createComment('Value Range : '+str(item.valueRange).strip().replace('\\n',',').replace('\n',',').replace('\r',''))
                            parentXmlNode.appendChild(itemCmmt)

                        paramXmlNode = dom.createElement(item.nameReplace.strip())
                        parentXmlNode.appendChild(paramXmlNode)

                        itemDftValue = ""

                        itemDftValue = self.cPtnDftValueGen(item)[1:-1].replace('\\"','"').replace('\\n','\n')

                        #logger.debug("default value ::: "+itemDftValue)

                        theDftValue = ''
                        if itemDftValue.strip().startswith('['):
                            theInstList = parentInstanceNumList.strip().split(',')

                            theInstArray = '['
                            for instNum in theInstList:
                                if instNum != '0':
                                    if theInstArray == '[':
                                        theInstArray = instNum
                                    else:
                                        theInstArray = theInstArray+','+instNum
                                else:
                                    pass

                            if theInstArray == '[':
                                #no instance found
                                logger.debug("no default value found")
                                continue
                            else:
                                theInstArray = theInstArray+']='

                            if not itemDftValue.find(theInstArray)==-1:
                                theDftValue = itemDftValue[itemDftValue.find(theInstArray)+len(theInstArray):]
                            elif not itemDftValue.find(']=')==-1:
                                theDftValue = itemDftValue[itemDftValue.find(']=')+len(']='):]
                            else:
                                continue

                            if not theDftValue.find('[')==-1:
                                theDftValue = theDftValue[:theDftValue.find('[')]

                            if theDftValue.startswith('"'):
                                theDftValue = theDftValue[1:]

                            if theDftValue.endswith('"'):
                                theDftValue = theDftValue[:-1]

                        else:
                            theDftValue = itemDftValue

                        if len(theDftValue) > 0:
                            txtNode = dom.createTextNode(theDftValue)
                            paramXmlNode.appendChild(txtNode)

            else:
                #logger.debug(item.nodeFullNameReplace+' not exists in xml by default')
                pass

        if not os.path.exists("./db_xml/"):
            os.makedirs("./db_xml/")

        tDbVersion = ''
        if self.dbVersion.has_key(dbname):
            tDbVersion = self.dbVersion[dbname]

        logger.debug("./db_xml/" + str(dbname).capitalize() + "_"+tDbVersion+".xml generated")
        f = open("./db_xml/" + str(dbname).capitalize() + "_"+tDbVersion+".xml","wb")
        dom.writexml(f, addindent='  ', newl='\n', encoding='utf-8')
        f.close()

    def doIndent(dom, node, indent = 0):
        children = node.childNodes[:]
        if indent:
            text = dom.createTextNode('\n' + ' '* 2 * indent)
            node.parentNode.insertBefore(text, node)

        if children:
            if children[-1].nodeType == node.ELEMENT_NODE:
                text = dom.createTextNode('\n' + ' ' * 2 * indent)
                node.appendChild(text)

            for n in children:
                if n.nodeType == node.ELEMENT_NODE:
                    self.doIndent(dom, n, indent + 1)

class ExcelParser:
    """the class to parser the excel file"""
    def __init__(self, excelFilePath):
        #self.dbName=""
        self.models = []
        self.excelFilePath = excelFilePath
        logger.debug(': excelFilePath='+excelFilePath)
        self.excelBook = xlrd.open_workbook(self.excelFilePath)

        for sheet in self.excelBook.sheets():
            if sheet.name.endswith(" Model") or sheet.name.endswith(" ExtModel"):
                dbName = sheet.cell_value(2,0).replace('.','')
                self.models.append([sheet.name, dbName])

    def getDbName(self):
        return self.dbName

    def getModels(self):
        return self.models

class ProcessExcelFiles:
    def __init__(self, excelFilePath, excelFileName):
        self.excelFilePath = excelFilePath
        self.filename = excelFileName
        self.excelParser = ExcelParser(self.excelFilePath)

        self.targetPath="./results/"

        self.fwCmdDocInfo = ("Fwcmd","Description : Application configuration")
        self.fwMainInfo = [("MainVersion","","Description : main version","Data Type : string (INT8[])","Value Range : 64"),
                           ("VendorName","","Description : vendor name","Data Type : string (INT8[])","Value Range : 64"),
                           ("Description","","Description : the description about this version","Data Type : string (INT8[])","Value Range : 1024"),
                           ("ModuleNumberOfEntries","1","Description : number of modules","Data Type : unsignedInt (UINT32)","Value Range : ")]
        self.fwMainModule = [("Module","","Description : module entity info","Data Type : object (STRUCT)","Value Range : 30"),
                             ("Name","","Description : the name of module","Data Type : string (INT8[])","Value Range : 32"),
                             ("Version","","Description : the version of this module","Data Type : string (INT8[])","Value Range : 8"),
                             ("Operation","","Description : install/uninstall","Data Type : string (UINT32_ENUM)","Value Range : Install,Uninstall"),
                             ("SubModuleNumberOfEntries","","Description : number of modules","Data Type : unsignedInt (UINT32)","Value Range : ")]
        self.fwSubModule = [("SubModule","","Description : module entity info","Data Type : object (STRUCT)","Value Range : 30"),
                            ("Name","","Description : the name of module","Data Type : string (INT8[])","Value Range : 32"),
                            ("Version","","Description : the version of this module","Data Type : string (INT8[])","Value Range : 8"),
                            ("FileType","","Description : file type, maybe single-file, bootdata, kernel, or bootloader","Data Type : string (UINT32_ENUM)","Value Range : SingleFile,BootData,KernelImage,BootLoader"),
                            ("Path","","Description : where to find the binary or dynamic-library of this module","Data Type : string (INT8[])","Value Range : 128"),
                            ("Command","","Description : the command to start the module.\n1) thread: handle=dlopen(dynamic-lib-path);func=dlsym(handle, SmStartReq);\n2) process: ./tr69c -f LAN -o","Data Type : string (INT8[])","Value Range : 128"),
                            ("SignatureAlgo","","Description : md5 or others","Data Type : string (INT8[])","Value Range : 8"),
                            ("Signature","","Description : the signature of this image","Data Type : string (INT8[])","Value Range : 1024")]


    def OnGenerateFile(self,excelFileToXml,dbFileVersion):

        """ generate the files: source, header, pattern """

        dataProcesser = DataProcesser(self.excelParser.excelBook,dbFileVersion)

        for modelsheet,dbname in self.excelParser.models:
            if not excelFileToXml == 1:
                dataProcesser.genIncludeFile(self.targetPath, dbname, str(dbname).lower()+'_model_def.h')
                dataProcesser.genPatternFile(self.targetPath, dbname, str(dbname).lower()+'_model_pattern.c', str(dbname).lower()+'_model_def.h')
                dataProcesser.genRegisterFile(self.targetPath, dbname, str(dbname).lower()+'_model_def.h')
                dataProcesser.genDbMakeFile(self.targetPath, dbname, "all")
            else:
                dataProcesser.genDbDefaultXmlFile(self.targetPath, dbname)



    def OnExit(self, e):
        self.Close(True)


class firmwareProcesser:
    def __init__(self):
        self.fwCmdDocInfo = ("Fwcmd","Description : Application configuration")
        self.fwMainInfo = [("MainVersion","","Description : main version","Data Type : string (INT8[])","Value Range : 64"),
                           ("VendorName","","Description : vendor name","Data Type : string (INT8[])","Value Range : 64"),
                           ("Description","","Description : the description about this version","Data Type : string (INT8[])","Value Range : 1024"),
                           ("ModuleNumberOfEntries","1","Description : number of modules","Data Type : unsignedInt (UINT32)","Value Range : ")]
        self.fwMainModule = [("Module","","Description : module entity info","Data Type : object (STRUCT)","Value Range : 30"),
                             ("Name","","Description : the name of module","Data Type : string (INT8[])","Value Range : 32"),
                             ("Version","","Description : the version of this module","Data Type : string (INT8[])","Value Range : 8"),
                             ("Operation","Install","Description : install/uninstall","Data Type : string (UINT32_ENUM)","Value Range : Install,Uninstall"),
                             ("SubModuleNumberOfEntries","","Description : number of modules","Data Type : unsignedInt (UINT32)","Value Range : ")]
        self.fwSubModule = [("SubModule","","Description : module entity info","Data Type : object (STRUCT)","Value Range : 30"),
                            ("Name","","Description : the name of module","Data Type : string (INT8[])","Value Range : 32"),
                            ("Version","","Description : the version of this module","Data Type : string (INT8[])","Value Range : 8"),
                            ("FileType","","Description : file type, maybe single-file, bootdata, kernel, or bootloader","Data Type : string (UINT32_ENUM)","Value Range : SingleFile,BootData,KernelImage,BootLoader"),
                            ("Path","","Description : where to find the binary or dynamic-library of this module","Data Type : string (INT8[])","Value Range : 128"),
                            ("Command","","Description : the command to start the module.\n1) thread: handle=dlopen(dynamic-lib-path);func=dlsym(handle, SmStartReq);\n2) process: ./tr69c -f LAN -o","Data Type : string (INT8[])","Value Range : 128"),
                            ("SignatureAlgo","","Description : md5 or others","Data Type : string (INT8[])","Value Range : 8"),
                            ("Signature","","Description : the signature of this image","Data Type : string (INT8[])","Value Range : 1024")]

    def OnGenerateDataFirmware(self,toDefault):
        logger.debug("generating data packet for firmware, toDefault="+str(toDefault))
        impl = xml.dom.minidom.getDOMImplementation()

        nameDoc,commentDoc = self.fwCmdDocInfo

        dom = impl.createDocument(None, nameDoc, None)
        root = dom.documentElement
        cmmt = dom.createComment(commentDoc)
        dom.insertBefore(cmmt,root)

        # fwMainInfo
        for mainName,mainValue,mainDescription,mainType,mainRange in self.fwMainInfo:
            cmmt = dom.createComment(mainDescription)
            root.appendChild(cmmt)
            cmmt = dom.createComment(mainType)
            root.appendChild(cmmt)
            cmmt = dom.createComment(mainRange)
            root.appendChild(cmmt)

            paramNode = dom.createElement(mainName)
            root.appendChild(paramNode)

            txtNode = dom.createTextNode(mainValue)
            paramNode.appendChild(txtNode)

        # fwMainModule
        mainModName,mainModValue,mainModDescription,mainModType,mainModRange = self.fwMainModule[0]

        cmmt = dom.createComment(mainModDescription)
        root.appendChild(cmmt)
        cmmt = dom.createComment(mainModType)
        root.appendChild(cmmt)
        cmmt = dom.createComment(mainModRange)
        root.appendChild(cmmt)

        paramNode = dom.createElement(mainModName)
        root.appendChild(paramNode)

        # i1
        subNode = dom.createElement("i1")
        paramNode.appendChild(subNode)

        # name
        mainModName,mainModValue,mainModDescription,mainModType,mainModRange = self.fwMainModule[1]
        cmmt = dom.createComment(mainModDescription)
        subNode.appendChild(cmmt)
        cmmt = dom.createComment(mainModType)
        subNode.appendChild(cmmt)
        cmmt = dom.createComment(mainModRange)
        subNode.appendChild(cmmt)

        paramNode = dom.createElement(mainModName)
        subNode.appendChild(paramNode)

        subname = ''
        if toDefault==1:
            subname = 'defaultdata'
        else:
            subname = 'data'

        txtNode = dom.createTextNode(subname)
        paramNode.appendChild(txtNode)

        # sub-version
        mainModName,mainModValue,mainModDescription,mainModType,mainModRange = self.fwMainModule[2]
        cmmt = dom.createComment(mainModDescription)
        subNode.appendChild(cmmt)
        cmmt = dom.createComment(mainModType)
        subNode.appendChild(cmmt)
        cmmt = dom.createComment(mainModRange)
        subNode.appendChild(cmmt)

        paramNode = dom.createElement(mainModName)
        subNode.appendChild(paramNode)

        txtNode = dom.createTextNode(mainModValue)
        paramNode.appendChild(txtNode)

        # operation
        mainModName,mainModValue,mainModDescription,mainModType,mainModRange = self.fwMainModule[3]
        cmmt = dom.createComment(mainModDescription)
        subNode.appendChild(cmmt)
        cmmt = dom.createComment(mainModType)
        subNode.appendChild(cmmt)
        cmmt = dom.createComment(mainModRange)
        subNode.appendChild(cmmt)

        paramNode = dom.createElement(mainModName)
        subNode.appendChild(paramNode)

        txtNode = dom.createTextNode(mainModValue)
        paramNode.appendChild(txtNode)

        # SubModuleNumberOfEntries
        mainModName,mainModValue,mainModDescription,mainModType,mainModRange = self.fwMainModule[4]
        cmmt = dom.createComment(mainModDescription)
        subNode.appendChild(cmmt)
        cmmt = dom.createComment(mainModType)
        subNode.appendChild(cmmt)
        cmmt = dom.createComment(mainModRange)
        subNode.appendChild(cmmt)

        paramNode = dom.createElement(mainModName)
        subNode.appendChild(paramNode)

        subModCnt = 0

        for fileName in os.listdir("./db_xml/"):
            if fileName.strip().lower().endswith(".xml"):
                subModCnt = subModCnt+1

        txtNode = dom.createTextNode(str(subModCnt))
        paramNode.appendChild(txtNode)


        xmlfilelist = 'Fwcmd.xml'

        if subModCnt > 0:
            fwSubName,fwSubValue,fwSubDescription,fwSubType,fwSubRange = self.fwSubModule[0]

            cmmt = dom.createComment(fwSubDescription)
            subNode.appendChild(cmmt)
            cmmt = dom.createComment(fwSubType)
            subNode.appendChild(cmmt)
            cmmt = dom.createComment(fwSubRange)
            subNode.appendChild(cmmt)

            itemNode = dom.createElement(fwSubName)
            subNode.appendChild(itemNode)

            subModCnt = 0

            for fileName in os.listdir("./db_xml/"):
                if fileName.strip().lower().endswith(".xml") and not fileName.strip().lower().startswith('cndata'):
                    #i1,i2,i3...
                    subModCnt = subModCnt + 1
                    itemSubNode = dom.createElement("i"+str(subModCnt))
                    itemNode.appendChild(itemSubNode)

                    xmlfilelist = xmlfilelist +' '+fileName

                    #name
                    fwSubName,fwSubValue,fwSubDescription,fwSubType,fwSubRange = self.fwSubModule[1]
                    cmmt = dom.createComment(fwSubDescription)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubType)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubRange)
                    itemSubNode.appendChild(cmmt)

                    paramNode = dom.createElement(fwSubName)
                    itemSubNode.appendChild(paramNode)

                    txtNode = dom.createTextNode(fileName)
                    paramNode.appendChild(txtNode)

                    #version
                    fwSubName,fwSubValue,fwSubDescription,fwSubType,fwSubRange = self.fwSubModule[2]
                    cmmt = dom.createComment(fwSubDescription)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubType)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubRange)
                    itemSubNode.appendChild(cmmt)

                    paramNode = dom.createElement(fwSubName)
                    itemSubNode.appendChild(paramNode)

                    fileVersion = fileName.strip().lower()[:-4]

                    if not fileVersion.find('_')==-1:
                        fileVersion = fileVersion[fileVersion.find('_')+1:]
                    else:
                        pass

                    #txtNode = dom.createTextNode(fileVersion)
                    #paramNode.appendChild(txtNode)

                    #filetype
                    fwSubName,fwSubValue,fwSubDescription,fwSubType,fwSubRange = self.fwSubModule[3]
                    cmmt = dom.createComment(fwSubDescription)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubType)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubRange)
                    itemSubNode.appendChild(cmmt)

                    paramNode = dom.createElement(fwSubName)
                    itemSubNode.appendChild(paramNode)

                    txtNode = dom.createTextNode('SingleFile')
                    paramNode.appendChild(txtNode)

                    #path
                    fwSubName,fwSubValue,fwSubDescription,fwSubType,fwSubRange = self.fwSubModule[4]
                    cmmt = dom.createComment(fwSubDescription)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubType)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubRange)
                    itemSubNode.appendChild(cmmt)

                    paramNode = dom.createElement(fwSubName)
                    itemSubNode.appendChild(paramNode)

                    workingPath = ''
                    if toDefault:
                        workingPath = '/data/'+fileName
                        workingPath = workingPath.replace('_'+fileVersion,'')+'.default'
                    else:
                        workingPath = '/data/'+fileName
                        workingPath = workingPath.replace('_'+fileVersion,'')+'.replace'

                    txtNode = dom.createTextNode(workingPath)
                    paramNode.appendChild(txtNode)

                    #command
                    fwSubName,fwSubValue,fwSubDescription,fwSubType,fwSubRange = self.fwSubModule[5]
                    cmmt = dom.createComment(fwSubDescription)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubType)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubRange)
                    itemSubNode.appendChild(cmmt)

                    paramNode = dom.createElement(fwSubName)
                    itemSubNode.appendChild(paramNode)

                    txtNode = dom.createTextNode('')
                    paramNode.appendChild(txtNode)

                    #signatureAlgo, leave the text empty because we have no private key to signature anything.
                    fwSubName,fwSubValue,fwSubDescription,fwSubType,fwSubRange = self.fwSubModule[6]
                    cmmt = dom.createComment(fwSubDescription)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubType)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubRange)
                    itemSubNode.appendChild(cmmt)

                    paramNode = dom.createElement(fwSubName)
                    itemSubNode.appendChild(paramNode)

                    txtNode = dom.createTextNode("")
                    paramNode.appendChild(txtNode)

                    #signature, leave the text empty because we have no private key to signature anything.
                    fwSubName,fwSubValue,fwSubDescription,fwSubType,fwSubRange = self.fwSubModule[7]
                    cmmt = dom.createComment(fwSubDescription)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubType)
                    itemSubNode.appendChild(cmmt)
                    cmmt = dom.createComment(fwSubRange)
                    itemSubNode.appendChild(cmmt)

                    paramNode = dom.createElement(fwSubName)
                    itemSubNode.appendChild(paramNode)

                    txtNode = dom.createTextNode("")
                    paramNode.appendChild(txtNode)

        if os.path.exists("./db_xml/Fwcmd.xml"):
            os.remove("./db_xml/Fwcmd.xml")

        cnfFile = open("./db_xml/Fwcmd.xml",  'wb')
        dom.writexml(cnfFile, encoding='utf-8')
        cnfFile.close()

        if os.path.exists('./'+subname+'.tgz'):
            os.remove('./'+subname+'.tgz')

        fwCmd = 'cd ./db_xml;tar zcvf ../'+subname+'.tgz '+xmlfilelist+';rm -rf Fwcmd.xml'
        logger.debug(fwCmd)

        os.system(fwCmd)

def main():
    """the main function"""
    global ExcelFileDir
    global CommonSrcDir
    global ResultSrcDir
    global SelfDefinedPrefix
    global WorkingDir
    global LoadingDir
    global BackupDir
    global MappingXmlDir

    excelFileToXml = 0
    excelFileToDefaultData = 0
    excelFileToData = 0
    excelFileDirList = []
    dbFileVersions = {}
    convertCurl = 0
    configFile = './genFilesByDM.conf'
    ArgDefPrefix = "X_VENDOR_"

    #initialize log file
    logfile = os.getcwd().strip()+'/log.txt'
    if os.path.exists(logfile):
        os.remove(logfile)
    else:
        pass

    for argU in sys.argv[1:]:
        if argU.strip() == "help":
            logger.debug("Without arguments, this python script only parse excel files at ./excel_files/ and genenrate relevant source code at ../db_xxx/ folder.")
            logger.debug("The script also will parse ./DbConfig.conf to decide the prefix of self-defined parameters and default DB file location.")
            logger.debug("You can inidcate these arguments below to change the configuration of the script.")
            logger.debug("Config=xxx : using xxx configuration file. ")
            logger.debug("genXml : generate xml files")
            logger.debug("genData : generate xml files and make it as part of firmware tarball")
            logger.debug("genDefaultData : generate default xml files and make it as part of firmware tarball")
            logger.debug("excel-file-path : indicate excel file dir or path, e,g, ./excel_files/DataModel.xls")
            logger.debug("dbVersion[DbName,Version] : change the version of Data Model, e,g, dbVersion[Device,2578]")
            logger.debug("SelfDefinedPrefix=X_VENDOR_ : indicate self-defined prefix")
            exit(0);
        elif argU.strip().startswith("Config="):
            configFile = argU.strip()[len('Config='):].strip()
            logger.debug("Configuration file is specified with : " + argU)
        elif argU.strip() == "genXml":
            excelFileToXml =1
            logger.debug("Xml generation needed...")
        elif argU.strip().startswith("genDefaultData"):
            excelFileToDefaultData =1
            excelFileToXml =1
            logger.debug("Xml & Default-data-firmware generation needed...")
        elif argU.strip().startswith("genData"):
            excelFileToData =1
            excelFileToXml =1
            logger.debug("Xml & Data-firmware generation needed...")
        elif os.path.isdir(argU):
            excelFileDirList.append(os.path.abspath(argU))
            logger.debug("excel dir specified at "+argU)
        elif os.path.isfile(argU):
            excelFileDirList.append(os.path.abspath(argU))
            logger.debug("excel file specified at "+argU)
        elif argU.strip().startswith("dbVersion["):
            #format : xmlVersion[Device,2578]
            dbName = argU.strip()[argU.strip().find('[')+1:argU.strip().find(',')]
            dbVersion = argU.strip()[argU.strip().find(',')+1:argU.strip().find(']')]
            logger.debug("db version specified : "+argU+" => excelFile="+dbName+" dbVersion="+dbVersion)
            dbFileVersions[dbName]=dbVersion
        elif argU.strip().startswith("SelfDefinedPrefix="):
            ArgDefPrefix = argU.strip()[len('SelfDefinedPrefix='):]
            logger.debug("Self-Defined Prefix indicated : "+argU)
        elif argU.strip() == "convertCurl":
            convertCurl = 1

    if convertCurl ==1:
        logger.debug("convert curl code to c-array")
        f = open('./curl.src','rb')
        codebuf = f.read()
        f.close()
        f = open('./curl.dst','wb')
        linearray = codebuf.strip().replace('\r','').split('\n')
        for oneline in linearray:
            if len(oneline)>0:
                if oneline.isdigit()==True:
                    f.write('\t{'+oneline+', "')
                else:
                    f.write(oneline.replace('"','\\"')+'"},\n')
            else:
                pass
        f.close()
        exit(1)
    else:
        pass

    # read config
    if (parseConfiguration(configFile)):
        os.chdir(os.path.dirname(configFile))

    if ArgDefPrefix != "X_VENDOR_":
        SelfDefinedPrefix = ArgDefPrefix

    if len(excelFileDirList) <= 0:
        excelFileDirList.append(ExcelFileDir)
        logger.debug("no excel file specified, use " + ExcelFileDir + " folder by default...")
    else :
        ExcelFileDir = excelFileDirList

    printConfig()

    # remove all generated files
    rmCmd = "rm -rf "+CommonSrcDir + "/dm_lib/db_*"
    logger.debug(rmCmd)
    os.system(rmCmd)

    # parse excel & generate file
    for excelFiles in excelFileDirList:
        if os.path.isdir(excelFiles):
            for fileName in os.listdir(excelFiles):
                logger.debug(": check "+fileName)
                fileFullName=os.path.join(excelFiles,fileName)
                if os.path.isfile(fileFullName):
                    if str(fileName).strip().lower().endswith('.xls'):
                        logger.debug(": parsing "+fileName)
                        dbfile = ProcessExcelFiles(fileFullName,fileName)
                        dbFileVersion = ''
                        try:
                            if dbFileVersions[fileName] != '':
                                dbFileVersion = dbFileVersions[fileName]
                            else:
                                pass
                        except:
                            pass
                        logger.debug(fileName+" : external dbFileVersion '"+dbFileVersion+"'")
                        dbfile.OnGenerateFile(excelFileToXml,dbFileVersion)
        elif str(excelFiles).strip().lower().endswith('.xls'):
            logger.debug(": parsing single file "+excelFiles)

            excelFileName = excelFiles
            if excelFiles.rfind('/') != -1:
                excelFileName = excelFiles[excelFiles.rfind('/')+1:]
            else:
                pass

            dbfile = ProcessExcelFiles(excelFiles,excelFileName)

            dbFileVersion = ''
            try:
                if dbFileVersions[excelFileName] != '':
                    dbFileVersion = dbFileVersions[excelFileName]
                else:
                    pass
            except:
                pass
            logger.debug(excelFileName+" : external dbFileVersion '"+dbFileVersion+"'")
            dbfile.OnGenerateFile(excelFileToXml,dbFileVersion)

    fwHandler = firmwareProcesser()

    if excelFileToData == 1:
        fwHandler.OnGenerateDataFirmware(0)

    if excelFileToDefaultData == 1:
        fwHandler.OnGenerateDataFirmware(1)

    if not excelFileToXml == 1 and not excelFileToData == 1 and not excelFileToDefaultData == 1 and SelfDefinedPrefix.strip() :
        DmLibIncDir = CommonSrcDir + "/dm_lib/common/inc/"
        if not os.path.exists(DmLibIncDir):
            logger.debug(": [WARNING] dm_selfdefined.h try to be genenrated, but not found DM lib inc path " + DmLibIncDir)
        else :
            dmSelfDefineHeader = DmLibIncDir + "dm_selfdefined.h"
            if os.path.exists(dmSelfDefineHeader):
                os.remove(dmSelfDefineHeader)

            f = open(dmSelfDefineHeader,"wb")

            f.write("#ifndef DM_SELFDEFINED_H\n")
            f.write("#define DM_SELFDEFINED_H\n")
            f.write("\n"*2)
            f.write("#define DM_SELFDEFINED_PREFIX \""+SelfDefinedPrefix+"\"")
            f.write("\n"*2)
            f.write("#endif /* DM_SELFDEFINED_H */\n")
            f.close()

            logger.debug(": dm_selfdefined.h genenrated...")

    logger.debug(": generating data model source code finished")

#enter the main function
if __name__ == '__main__': main()
