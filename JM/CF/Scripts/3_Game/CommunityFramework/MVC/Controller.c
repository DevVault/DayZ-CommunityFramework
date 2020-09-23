/*

Controller Class Example:

class TestController: Controller
{
	// "TextBox1" goes into Binding_Name
	string TextBox1 = "TextBox1Text";
	
	// "TextBox2" goes into Binding_Name
	int TextBox2;
	
	// "WindowButton1" goes into Binding_Name
	bool WindowButton1; 
	
	
	// If WindowButton1 is bound to a button,
	// call this function and it will
	// set the state of the button on your UI
	void SetWindowButton1(bool state)
	{
		WindowButton1 = state;
		NotifyPropertyChanged("WindowButton1");
	}
	
	// Gets called whenever a property was changed in the controller
	override void PropertyChanged(string property_name)
	{
		switch (property_name) {
			
			case "WindowButton1": {
				Print("WindowButton1 " + WindowButton1);
				break;
			}
		}
	}
}
*/


// Abstract Class
class Controller: ScriptedViewBase
{
	// All View Bindings
	protected ref ViewBindingHashMap m_ViewBindingHashMap;
	
	// View Bindings indexed by their Binding_Name
	protected ref DataBindingHashMap m_DataBindingHashMap;
	DataBindingHashMap GetDataBindings() {
		return m_DataBindingHashMap;
	}
	
	protected ref PropertyTypeHashMap m_PropertyTypeHashMap;
	typename GetPropertyType(string property_name) {
		return m_PropertyTypeHashMap.Get(property_name);
	}
	
	void Controller() 
	{
		MVC.Trace("Controller"); 
		
		m_ViewBindingHashMap = new ViewBindingHashMap();
		m_DataBindingHashMap = new DataBindingHashMap();
		
		m_PropertyTypeHashMap = PropertyTypeHashMap.FromType(Type());
		m_PropertyTypeHashMap.RemoveType(Controller);
	}
	
	void ~Controller() 
	{ 
		MVC.Trace("~Controller"); 
		
		delete m_ViewBindingHashMap;
		delete m_DataBindingHashMap;
		delete m_PropertyTypeHashMap;
	}
	
	void OnWidgetScriptInit(Widget w)
	{
		m_LayoutRoot = w;
		m_LayoutRoot.SetHandler(this);
		MVC.Trace("Controller::OnWidgetScriptInit %1", m_LayoutRoot.GetName());
		
		// Load all child Widgets and obtain their DataBinding class
		int binding_count = LoadDataBindings(m_LayoutRoot);
		MVC.Log("%1: %2 DataBindings found!", m_LayoutRoot.GetName(), binding_count.ToString());	
	}

		
	// Call this when you update a Controller property (variable)
	// Do NOT call this when using arrays / collections.
	// Use ObservableCollection!
	void NotifyPropertyChanged(string property_name)
	{
		MVC.Trace("Controller::NotifyPropertyChanged " + property_name);
		ViewBindingArray views = m_DataBindingHashMap.Get(property_name);
		
		if (views) 
			foreach (ViewBinding view: views)
				view.UpdateView();

		PropertyChanged(property_name);
	}
	
	
	// This gets called automatically when a collection is changed
	void NotifyCollectionChanged(string collection_name, CollectionChangedEventArgs args)
	{		
		MVC.Trace("Controller::NotifyCollectionChanged %1", collection_name);
		ViewBindingArray views = m_DataBindingHashMap.Get(collection_name);
				
		if (views) 
			foreach (ViewBinding view: views) 
				view.OnCollectionChanged(args);

		CollectionChanged(collection_name, args);
	}
	
	// Gets called every time a property is changed. 
	// Override this when you want to have an event AFTER property is changed
	void PropertyChanged(string property_name);
	
	// Gets called every time an observable collection is changed.
	// Override this when you want to have an event AFTER collection is changed
	void CollectionChanged(string collection_name, CollectionChangedEventArgs args);
	
	
	int LoadDataBindings(Widget w)
	{
		ViewBinding view_binding;
		w.GetScript(view_binding);
		
		if (view_binding && (view_binding.IsInherited(ViewBinding))) {
			m_ViewBindingHashMap.Insert(w, view_binding);
			m_DataBindingHashMap.InsertView(view_binding);
			view_binding.SetController(this);
		}
		
		if (w.GetChildren() != null)
			LoadDataBindings(w.GetChildren());
		
		
		if (w.GetSibling() != null) 
			LoadDataBindings(w.GetSibling());
		
		return m_DataBindingHashMap.Count();
	}
}











