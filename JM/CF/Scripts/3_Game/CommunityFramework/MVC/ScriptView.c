
// Abstract Class
// Inherit from this if you want to create UI elements with 100% script
/*

Example:

class CustomDialogWindow: ScriptView
{
	override string GetLayoutFile() {
		return "MyMod/gui/Layouts/dialogs/Dialog.layout";
	}
}

	CustomDialogWindow window = new CustomDialogWindow();
	window.Show();
	....
	....
	window.Close();

*/

class ScriptView: ScriptedViewBase
{
	protected ScriptView m_ParentView;
	protected ref Controller m_Controller;
	
	override ScriptedViewBase GetParent() {
		return null;
	}
	
	Controller GetController() {
		return m_Controller;
	}
	
	protected ref CommandManager m_CommandManager;
	CommandManager GetCommandManager() {
		return m_CommandManager;
	}
	
	void ScriptView(ScriptView parent = null)
	{
		MVC.Trace("ScriptView");
		m_ParentView = parent;
		
		PropertyTypeHashMap property_map = PropertyTypeHashMap.FromType(Type());
		property_map.RemoveType(ScriptView);		
				
		if (!GetLayoutFile()) {
			MVC.Error("ScriptView: You must override GetLayoutFile with the .layout file path");
			return;
		}
		
		MVC.Log("ScriptView: Loading %1", GetLayoutFile());
		WorkspaceWidget workspace = GetWorkbenchGame().GetWorkspace();
		if (!workspace) {
			MVC.Error("ScriptView: Workspace was null, try reloading Workbench");
			return;
		}
		
		m_LayoutRoot = workspace.CreateWidgets(GetLayoutFile(), m_ParentView.GetLayoutRoot());
		if (!m_LayoutRoot) {
			MVC.Error("ScriptView: Invalid layout file %1", GetLayoutFile());
			return;
		}
		
		int property_count;
		foreach (string property_name, typename property_type: property_map) {
			
			Widget target = m_LayoutRoot.FindAnyWidget(property_name);
			
			// Allows for LayoutRoot to be referenced as well
			if (!target && m_LayoutRoot.GetName() == property_name) {
				target = m_LayoutRoot;
			}

			EnScript.SetClassVar(this, property_name, 0, target);
			property_count++;
		}
		
		MVC.Log("ScriptView: %1 properties found!", property_count.ToString());
		
		// Has to be called before other views are created
		//m_LayoutRoot.SetHandler(this);
		// You can keep the controller in scriptclass if you want, to keep reactive UI's up
		m_LayoutRoot.GetScript(m_Controller);
		
		if (!m_Controller && GetControllerType().IsInherited(Controller)) {
			m_Controller = GetControllerType().Spawn();
			if (!m_Controller) {
				MVC.Error("ScriptView: Invalid Controller %1", GetControllerType().ToString());
				return;
			}
			
			m_Controller.OnWidgetScriptInit(m_LayoutRoot);	
		}
		
		m_Controller.SetScriptView(this);
		
		m_CommandManager = GetCommandManagerType().Spawn();
		
		
		set<ViewBinding> view_bindings = m_Controller.GetViewBindings();
		foreach (ViewBinding view_binding: view_bindings) {
			//RoutedUICommand routed_command = view_binding.GetRoutedUICommand();
			//RoutedCommands.Insert(routed_command.KeyStorageValue(), routed_command);
		}
	}
	
	void ~ScriptView() 
	{
		MVC.Trace("~ScriptView");
		m_LayoutRoot.Unlink();
		delete m_Controller;
	}

	void OnWidgetScriptInit(Widget w)
	{
		// Extra safety measure
#ifdef COMPONENT_SYSTEM
		MVC.Error("ScriptView should NOT be called in the Workbench Script Class!");
		return;
#endif
	}
					
	// Abstract Methods
	protected string GetLayoutFile();
	
	protected typename GetControllerType() {
		return Controller;
	}
	
	protected typename GetCommandManagerType() {
		return CommandManager;
	}
}
