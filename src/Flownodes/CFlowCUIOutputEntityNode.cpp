#include <StdAfx.h>
#include <Nodes/G2FlowBaseNode.h>
#include <CPluginCoherentUI.h>
#include <Coherent/UI/View.h>
#include "CoherentViewListener.h"
#include "CoherentUISystem.h"
#include "ViewConfig.h"

namespace CoherentUIPlugin
{
    class CFlowCUIOutputEntityNode : public CFlowBaseNode<eNCT_Instanced>
    {
        private:
            CCoherentViewListener* m_pViewListener;
            IEntity* m_pEntity;
            ViewConfig* m_pViewConfig;
            bool m_bViewNeedsUpdate;

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
                EIP_URL,
                EIP_WIDTH,
                EIP_HEIGHT,
                EIP_TRANSPARENT,
                EIP_CLICKABLE,
                EIP_MESH,
                EIP_SHARED_MEMORY,
            };

            enum EOutputPorts
            {
                EOP_VIEWID = 0
            };

#define INITIALIZE_OUTPUTS(x) \
    ActivateOutput<int>(x, EOP_VIEWID, -1);

        public:
            CFlowCUIOutputEntityNode( SActivationInfo* pActInfo )
            {
                m_pViewListener = NULL;
                m_pEntity = NULL;
                m_pViewConfig = new ViewConfig();
                m_bViewNeedsUpdate = false;
            }

            virtual ~CFlowCUIOutputEntityNode()
            {
                if ( m_pViewListener )
                {
                    gCoherentUISystem->DeleteView( m_pViewListener );
                }
                if ( m_pViewConfig )
                {
                    delete m_pViewConfig;
                }
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CFlowCUIOutputEntityNode( pActInfo );
            }

            virtual void GetMemoryUsage( ICrySizer* s ) const
            {
                s->Add( *this );
            }

            void Serialize( SActivationInfo* pActInfo, TSerialize ser )
            {
            }

            virtual void GetConfiguration( SFlowNodeConfig& config )
            {
                static const SInputPortConfig inputs[] =
                {
                    InputPortConfig_Void( "Activate",                            _HELP( "Activate View" ) ),
                    InputPortConfig<string>( "Url",                  "",         _HELP( "Initial URL" ) ),
                    InputPortConfig<int>( "Width",                   0,          _HELP( "Width" ) ),
                    InputPortConfig<int>( "Height",                  0,          _HELP( "Height" ) ),
                    InputPortConfig<bool>( "Transparent",        false,          _HELP( "Is Transparent" ) ),
                    InputPortConfig<bool>( "Clickable",           true,          _HELP( "Is Clickable" ) ),
                    InputPortConfig<string>( "Mesh",                "",          _HELP( "Collision Mesh" ) ),
                    InputPortConfig<bool>( "SharedMemory",        true,          _HELP( "Uses Shared Memory" ) ),
                    InputPortConfig_AnyType( NULL ),
                };

                static const SOutputPortConfig outputs[] =
                {
                    OutputPortConfig<int>( "ViewID",                             _HELP( "ID for Further Use" ),                        "nViewID" ),
                    OutputPortConfig_AnyType( NULL ),
                };

                config.pInputPorts = inputs;
                config.pOutputPorts = outputs;
                config.sDescription = _HELP( PLUGIN_CONSOLE_PREFIX "CoherentUI on entity" );

                config.nFlags |= EFLN_TARGET_ENTITY;
                config.SetCategory( EFLN_APPROVED );
            }

            virtual void ProcessEvent( EFlowEvent evt, SActivationInfo* pActInfo )
            {
                switch ( evt )
                {
                    case eFE_Suspend:
                        break;

                    case eFE_Resume:
                        break;

                    case eFE_Initialize:
                        INITIALIZE_OUTPUTS( pActInfo );
                        break;

                    case eFE_SetEntityId:
                        m_pEntity = pActInfo->pEntity;
                        break;

                    case eFE_Activate:
                        if ( IsPortActive( pActInfo, EIP_ACTIVATE ) && m_pEntity )
                        {
                            // get the view definition
                            std::string sUrl = GetPortString( pActInfo, EIP_URL );
                            std::wstring sUrlW( sUrl.length(), L' ' );
                            sUrlW.assign( sUrl.begin(), sUrl.end() );

                            Coherent::UI::ViewInfo info;
                            info.Width = GetPortInt( pActInfo, EIP_WIDTH );
                            info.Height = GetPortInt( pActInfo, EIP_HEIGHT );
                            info.IsTransparent = GetPortBool( pActInfo, EIP_TRANSPARENT );
                            info.UsesSharedMemory = GetPortBool( pActInfo, EIP_SHARED_MEMORY );
                            info.SupportClickThrough = GetPortBool( pActInfo, EIP_CLICKABLE );

                            m_pViewConfig->ViewInfo = info;
                            m_pViewConfig->Url = sUrlW;
                            m_pViewConfig->Entity = m_pEntity;
                            m_pViewConfig->CollisionMesh = GetPortString( pActInfo, EIP_MESH );

                            // indicate that we have to create/update the view later
                            m_bViewNeedsUpdate = true;
                            pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, true );
                        }

                        break;

                    case eFE_Update:
                        // make sure the view is created/updated, after the system is ready
                        if ( m_bViewNeedsUpdate &&  gCoherentUISystem->IsReady() )
                        {
                            if ( m_pViewListener )
                            {
                                gCoherentUISystem->DeleteView( m_pViewListener );
                            }
                            m_pViewListener = gCoherentUISystem->CreateView( m_pViewConfig );
                            m_bViewNeedsUpdate = false;
                        }

                        // set the view id output after the view is available
                        if ( m_pViewListener )
                        {
                            Coherent::UI::View* view = m_pViewListener->GetView();
                            if ( view )
                            {
                                ActivateOutput<int>( pActInfo, EOP_VIEWID, view->GetId() );
                                // updates are not necessary until next initialization
                                pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, false );
                            }
                        }
                        break;
                }
            }
    };
}

REGISTER_FLOW_NODE_EX( "CoherentUI_Plugin:OutputEntity", CoherentUIPlugin::CFlowCUIOutputEntityNode, CFlowCUIOutputEntityNode );