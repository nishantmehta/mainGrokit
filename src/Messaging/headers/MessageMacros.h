//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#ifndef MESSAGE_MACROS
#define MESSAGE_MACROS

////////////////////////////////////////////////
// MACRO DEFINITIONS TO STREAAMLINE MESSAGE HANDLING

// macro to declare a message handler
#define MESSAGE_HANDLER_DECLARATION(MessageHandler)\
static void MessageHandler(EventProcessorImp &_obj, Message &_msg);

// Converts a message to the given type
#define MESSAGE_CONVERT(inVar, outVar, MessageType) \
    bool _del_ptr = false; \
    MessageType * _msgPtr = NULL; \
    if( inVar.Serialized() ) { \
        _msgPtr = new MessageType(inVar.GetSerializedContent(), NULL, NULL, NULL); \
        _del_ptr = true; \
    } \
    else { \
        MessageType &tmp = dynamic_cast<MessageType&>(inVar); \
        _msgPtr = &tmp; \
    } \
    MessageType & outVar = *_msgPtr;

#define MESSAGE_CLEANUP \
    if( _del_ptr ) \
        delete _msgPtr;

// macro to start the definition of the MessageHandler in class ClassName to
// handle the message type MessageType
// at the end of invocation, evProc contains the object of type ClassName
// that received the message and msg contains the message (of type MessageType)
// all members of the object acted upon have to be prefixed by evProc.
#define MESSAGE_HANDLER_DEFINITION_BEGIN(ClassName, MessageHandler, MessageType) \
void ClassName::MessageHandler(EventProcessorImp &_obj, Message &_msg){\
    MESSAGE_CONVERT(_msg, msg, MessageType); \
    ClassName &evProc = dynamic_cast<ClassName&>(_obj);

// macro provided for uniformity
#define MESSAGE_HANDLER_DEFINITION_END \
    MESSAGE_CLEANUP; \
}

#define ACTOR_HANDLE                                                        \
virtual void _dispatch(Message &__msg) {

#define HANDLER(MESSAGE_TYPE,MESSAGE_HANDLER, PRIORITY)                     \
    if( registerMsg) {                                                      \
        RegisterMessagePriority(MESSAGE_TYPE::type, PRIORITY);              \
    }                                                                       \
    if( MESSAGE_TYPE::type == __msg.Type()) {                               \
        MESSAGE_CONVERT(__msg, temp, MESSAGE_TYPE)                          \
        MESSAGE_HANDLER(temp);                                              \
        return;}                                                            

#define  END_HANDLE_WITH_DEFAULT_HANDLER(DEFAULT_HANDLER)                   \
    if(registerMsg) {                                                       \
        RegisterMessagePriority(DieMessage::type, 0);                       \
    }                                                                       \
    if( DieMessage::type == __msg.Type() && !registerMsg) {                 \
        StopSpinning();                                                     \
        return;                                                             \
    } else {registerMsg = false;}                                           \
    DEFAULT_HANDLER(__msg);                                                 \
}

#define  END_HANDLE                                                         \
    if(registerMsg) {                                                       \
        RegisterMessagePriority(DieMessage::type, 0);                       \
    }                                                                       \
    if( DieMessage::type == __msg.Type() && !registerMsg) {                 \
        StopSpinning();                                                     \
        return;                                                             \
    } else {registerMsg = false; return;}                                   \
    DefaultMessageHandler(*this, __msg);                                    \
}

#endif
