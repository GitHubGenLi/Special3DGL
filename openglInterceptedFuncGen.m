clear all
close all
clc

%%

PATH_LOAD = 'H:\Private\MyProject\Matlab\IBit\'; %'C:\Users\txn\Documents\MyProject\';
PATH_SAVED = PATH_LOAD;

OPENGL_FILE = 'opengl_toAny.txt';
OPENGL_SIGNATURE_FILE = 'openglSign.txt';
INTERCEPTED_FUNC_HEADER_FILE = 'interceptedOpenglFuncHeader.txt';
INTERCEPTED_FUNC_BODY_FILE = 'interceptedOpenglFuncBody.txt';
HOOKED_FUNC_FILE = 'hookedFunc.txt';
UNHOOK_FUNC_FILE = 'unhookFunc.txt';

if exist(PATH_SAVED, 'dir')
    %warning(....)
else
    mkdir(PATH_SAVED)
end
%%
openglFullpath = [PATH_LOAD OPENGL_FILE];

fileOpenglID = fopen(openglFullpath);
opengl = textscan(fileOpenglID,'%s', 'Delimiter','\n');
fclose(fileOpenglID);

totalFunction = size(opengl{1,1});
%%
openglSignFullpath = [PATH_LOAD OPENGL_SIGNATURE_FILE];

fileOpenglSignID = fopen(openglSignFullpath);
openglSign = textscan(fileOpenglSignID,'%s', 'Delimiter','\n');
fclose(fileOpenglSignID);

%%
interceptedOpenglFuncHeaderFullpath = [PATH_LOAD INTERCEPTED_FUNC_HEADER_FILE];
fileinterceptedOpenglFuncHeaderID = fopen(interceptedOpenglFuncHeaderFullpath,'w');

%%
interceptedOpenglFuncBodyFullpath = [PATH_LOAD INTERCEPTED_FUNC_BODY_FILE];
fileinterceptedOpenglFuncBodyID = fopen(interceptedOpenglFuncBodyFullpath,'w');

%%
hookedFuncFullpath = [PATH_LOAD HOOKED_FUNC_FILE];
filehookedFuncID = fopen(hookedFuncFullpath,'w');

%%
unhookFuncFullpath = [PATH_LOAD UNHOOK_FUNC_FILE];
fileunhookFuncID = fopen(unhookFuncFullpath,'w');

%%
for index = 1: totalFunction(1)
    [token, remain] = strtok(opengl{1,1}(index), '=');
    token = strtrim(token);
    refinedFunc = textscan(token{1}, '_%s');
    realFunc = char(refinedFunc{1} (1));
    
    disp(['Processing function: ' realFunc]);
    
%     if (strcmp(realFunc, 'glFogf') > 0)
%         disp(['OK here it is the function: ' realFunc]);
%     end
    
    %get the function pointer using the first open/close bracket
    openBracket = strfind(opengl{1,1}(index), '(');
    closeBracket = strfind(opengl{1,1}(index), ')');
    
    firstOpen = openBracket{1,1}(1) + 1;
    firstClose = closeBracket{1,1}(1) - 1;
    oneLine = opengl{1,1}(index);
    funcPointer = oneLine{1}(firstOpen: firstClose);
    
    %get signature of the function
    ind = strfind(openglSign{1,1}, funcPointer);
    ind = find(~cellfun(@isempty, ind));
    funcFiltered = openglSign{1,1}(ind);
    indTypeddef = strfind(funcFiltered, 'typedef');
    indB = find(~cellfun(@isempty, indTypeddef));    
    
    %get the 1st one, assume that it is the line which contains the
    %signature of the function
    totalSign = size( indB);
    
    if (totalSign > 0)
        %funcDefined = funcFiltered(indB(1));
        
        for id = 1: totalSign
            funcDefined = funcFiltered(indB(id));
            
            openBracket = strfind(funcDefined, '(');
            closeBracket = strfind(funcDefined, ')');
        
            %use the 2nd open/close bracket to find the signature
            firstOpen = openBracket{1,1}(1);
            firstClose = closeBracket{1,1}(1);
        
            secondOpen = openBracket{1,1}(2);
            secondClose = closeBracket{1,1}(2);
        
            % %verify correct signature
            indexFuncPointer = strfind(funcDefined, funcPointer);
            verifiedFunctionPointer = funcDefined{1}(indexFuncPointer{1}:firstClose-1);
            
            if (strcmp(verifiedFunctionPointer, funcPointer) > 0)
                break;
            end
        end
        
        %get the signature of the function
        signature = funcDefined{1}(secondOpen : secondClose);
        listParams = funcDefined{1}(secondOpen + 1: secondClose - 1);
        
        %get the return value of the function
        %returnPart = funcDefined{1}(1 : firstOpen - 1);
        %realReturnPart = textscan(returnPart, 'typedef %s');
        realReturnPart = strtrim(char(funcDefined{1}(9 : firstOpen - 1))); %typedef has 8 characters
        
        %write full intercepted functions (including return value and
        %signature)
        signatureInterceptedFunc = strcat(realReturnPart, ' APIENTRY intercepted', realFunc, signature);
        signatureInterceptedFunc = char(strcat(signatureInterceptedFunc, ';'));
        nameInterceptedFunc = strcat('intercepted', realFunc);
        
        fullInterceptedFunction = strcat(realReturnPart, ' APIENTRY intercepted', realFunc, signature);
        fullInterceptedFunction = sprintf('%s\n{\n\t%s\n', fullInterceptedFunction, 'currentConfig3DSettings.increaseFunctionCall();');
        fullInterceptedFunction = sprintf('%s\n\t%s', fullInterceptedFunction, 'if (currentConfig3DSettings.startInterception())');
        fullInterceptedFunction = sprintf('%s\n\t{', fullInterceptedFunction);
        fullInterceptedFunction = sprintf('%s\n\t\t%s', fullInterceptedFunction, 'currentConfig3DSettings.getDrawingBuffer("');
        fullInterceptedFunction = strcat(fullInterceptedFunction, realFunc, '");');
        fullInterceptedFunction = sprintf('%s\n\t\t%s\n\t}', fullInterceptedFunction,'currentConfig3DSettings.switchCurrentBuffer();');
        
        if (strcmp(realReturnPart, 'void') == 0) %not void
            fullInterceptedFunction = sprintf('%s\n\treturn _%s(', fullInterceptedFunction, realFunc);    
        elseif (strcmp(realReturnPart, 'void') == 1)
            fullInterceptedFunction = sprintf('%s\n\t_%s(', fullInterceptedFunction, realFunc);    
        end
                
        %fill parameters if has: get each paramter's name
        remain = listParams;

        while true
           [params, remain] = strtok(remain, ',');
           if isempty(params),  break;  end
           
           params = strtrim(params);
           paramName = params;
           realparamName = paramName;
           
           while true
               [dataType, paramName] = strtok(paramName, ' ');
               
               paramName = strtrim(paramName);
               
               if isempty(paramName),  break;
               else
                   realparamName = paramName;
               end
           end
           
           if (strcmp(realparamName,'void') == 0) %not void
            fullInterceptedFunction = strcat(fullInterceptedFunction, realparamName, ',');
           end
           %disp(sprintf('%s: %s', params, paramName))
        end
        
        %remove the last , if has
        if (fullInterceptedFunction(length(fullInterceptedFunction)) == ',') 
            fullInterceptedFunction = fullInterceptedFunction(1: length(fullInterceptedFunction)-1);
        end
        %add the last ;
        fullInterceptedFunction = strcat(fullInterceptedFunction, ');');
        %close function
        %fullInterceptedFunction = sprintf('%s\n}\n', fullInterceptedFunction);
        fullInterceptedFunction = sprintf('%s\n}', fullInterceptedFunction);
        
        %write to the text file
        fprintf(fileinterceptedOpenglFuncHeaderID, '%s\n', signatureInterceptedFunc);
        fprintf(fileinterceptedOpenglFuncBodyID, '%s\n', fullInterceptedFunction);
        
        %create hooked function
        hookedFunc = strcat('if ((_', realFunc, ' == 0) || ');
        hookedFunc = sprintf('%s\n\t!Mhook_SetHook(reinterpret_cast<PVOID*>(&_%s),%s))', hookedFunc, realFunc, nameInterceptedFunc);
        hookedFunc = sprintf('%s\n{\n\t%s', hookedFunc, 'cerr << "Failed to hook _glLoadIdentity" << endl;');
        hookedFunc = sprintf('%s\n}\nelse\n{', hookedFunc);
        hookedFunc = sprintf('%s\n\tprintf("Hooked _%s\\n");\n', hookedFunc, realFunc);
        hookedFunc = sprintf('%s}', hookedFunc);
        
        %write hooked function to the text file
        fprintf(filehookedFuncID, '%s\n', hookedFunc);
        
        %unhook functions
        unhookfunction = strcat('Mhook_Unhook(reinterpret_cast<PVOID*>(&_', realFunc, '));');
        fprintf(fileunhookFuncID, '%s\n', unhookfunction);
        
    else
        disp('No signature for ' + funcPointer);
    end
end

%close all writing files
fclose(fileinterceptedOpenglFuncHeaderID);
fclose(fileinterceptedOpenglFuncBodyID);
fclose(filehookedFuncID);
fclose(fileunhookFuncID);

 disp('Finished!');
