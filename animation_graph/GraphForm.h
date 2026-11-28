#pragma once
#include "GraphAlgorithms.h"

namespace animationgraph {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class GraphForm : public System::Windows::Forms::Form
	{
	public:
		GraphForm(void)
		{
			InitializeComponent();
			InitializeVertexPositions();
			InitializeEdges();
			InitializeGraphModel();

			//initialize the animation order and visited vertices for Dijkstra's algorithm
			animationOrder =
				gcnew System::Collections::Generic::List<int>();

			animatedVisited =
				gcnew System::Collections::Generic::List<int>();

			//which vertex is currently being animated
			animationDistances = nullptr;
			//index of the current vertex being animated
			animationIndex = 0;
			currentAnimatedVertex = -1;	

			primParent = nullptr; //which parent connects each vertex to the MST
			primKey = nullptr; //the selected weight

			//edge currently highlighted during animation of Prim's
			mstEdgeSources =
				gcnew System::Collections::Generic::List<int>();

			mstEdgeDestinations =
				gcnew System::Collections::Generic::List<int>();

			//tells the timer whether its animation dijkstra or prims
			currentAnimationAlgorithm = "";
			//final MST weight
			primTotalWeight = 0;
		}



	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GraphForm()
		{
			//delete the graph model if it exists
			if (graphModel != nullptr)
			{
				delete graphModel;
				graphModel = nullptr;
			}

			//delete the animation order and visited vertices for Dijkstra's algorithm
			if (components)
			{
				delete components;
			}
		}

		//drawing of vertices and edges

	private:System::Collections::Generic::List<System::Drawing::Point>^ vertexPositions;
	private:
		System::Collections::Generic::List<int>^ edgeSources;
		System::Collections::Generic::List<int>^ edgeDestinations;
		System::Collections::Generic::List<int>^ edgeWeights;

		//animation order and visited vertices for Dijkstra's algorithm

	private:System::Collections::Generic::List<int>^ animationOrder;

		System::Collections::Generic::List<int>^ animatedVisited;

		array<int>^ animationDistances;

		int animationIndex;
		int currentAnimatedVertex;

		//animation order and visited vertices for Prim's algorithm

	private: array<int>^ primParent;
		   array<int>^ primKey;

		   System::Collections::Generic::List<int>^ mstEdgeSources;
		   System::Collections::Generic::List<int>^ mstEdgeDestinations;

		   System::String^ currentAnimationAlgorithm;
		   int primTotalWeight;

	private:
		void InitializeVertexPositions()
		{
			vertexPositions = gcnew System::Collections::Generic::List<System::Drawing::Point>();
			vertexPositions->Add(System::Drawing::Point(100, 300));
			vertexPositions->Add(System::Drawing::Point(280, 150));
			vertexPositions->Add(System::Drawing::Point(280, 450));
			vertexPositions->Add(System::Drawing::Point(500, 300));
			vertexPositions->Add(System::Drawing::Point(680, 300));
		}

	private:
		void InitializeEdges()
		{
			edgeSources =
				gcnew System::Collections::Generic::List<int>();

			edgeDestinations =
				gcnew System::Collections::Generic::List<int>();

			edgeWeights =
				gcnew System::Collections::Generic::List<int>();

			//og sample graph
			AddVisualEdge(0, 1, 4);
			AddVisualEdge(0, 2, 2);
			AddVisualEdge(1, 2, 1);
			AddVisualEdge(1, 3, 5);
			AddVisualEdge(2, 3, 8);
			AddVisualEdge(3, 4, 3);
		}


		void AddVisualEdge(int source, int destination, int weight)
		{
			edgeSources->Add(source);
			edgeDestinations->Add(destination);
			edgeWeights->Add(weight);
		}

	private:
		void DrawEdge(
			System::Drawing::Graphics^ graphics,
			int source,
			int destination,
			int weight)
		{
			System::Drawing::Point start = vertexPositions[source];
			System::Drawing::Point end = vertexPositions[destination];

			System::Drawing::Pen^ edgePen =
				gcnew System::Drawing::Pen(
					System::Drawing::Color::DarkGray,
					2.0f);

			graphics->DrawLine(edgePen, start, end);

			int middleX = (start.X + end.X) / 2;
			int middleY = (start.Y + end.Y) / 2;

			System::Drawing::SolidBrush^ weightBrush =
				gcnew System::Drawing::SolidBrush(
					System::Drawing::Color::DarkRed);

			graphics->DrawString(
				weight.ToString(),
				this->Font,
				weightBrush,
				middleX,
				middleY);

			delete edgePen;
			delete weightBrush;
		}

		private:
			void DrawMSTEdge(
				System::Drawing::Graphics^ graphics,
				int source, int destination) {
				System::Drawing::Point start = vertexPositions[source];

				System::Drawing::Point end = vertexPositions[destination];

				//blue and thicker than a normal graph edge
				System::Drawing::Pen^ mstPen =
					gcnew System::Drawing::Pen(
						System::Drawing::Color::Blue,
						5.0f);
				graphics->DrawLine(mstPen, start, end);

				delete mstPen;
			}

	private:
		void DrawVertex(
			System::Drawing::Graphics^ graphics,
			int vertex,
			System::Drawing::Color color)
		{
			int radius = 25;
			System::Drawing::Point position = vertexPositions[vertex];

			System::Drawing::Rectangle circle(
				position.X - radius,
				position.Y - radius,
				radius * 2,
				radius * 2);

			System::Drawing::SolidBrush^ vertexBrush =
				gcnew System::Drawing::SolidBrush(color);

			graphics->FillEllipse(vertexBrush, circle);
			graphics->DrawEllipse(
				System::Drawing::Pens::Black,
				circle);

			System::String^ label = vertex.ToString();

			System::Drawing::SizeF textSize =
				graphics->MeasureString(label, this->Font);

			graphics->DrawString(
				label,
				this->Font,
				System::Drawing::Brushes::Black,
				position.X - textSize.Width / 2,
				position.Y - textSize.Height / 2);

			delete vertexBrush;
		}

	private:void InitializeGraphModel()
		{
		//max of 100 vertices, undirected (false)
			graphModel = new Graph(10, false); // 10 vertices, undirected

			//create the five displayed sample vertices
			for (int vertex = 0;
				vertex < vertexPositions->Count;
				vertex++)
			{
				graphModel->createVertex();
			}

			//copy the displayed sample edges into the adjacency list of the graph model
			for (int edge = 0; edge < edgeSources->Count; edge++)
			{
				graphModel->createEdge(
					edgeSources[edge], 
					edgeDestinations[edge], 
					edgeWeights[edge]);
			}
		}

	private: Graph* graphModel;

	private: System::Windows::Forms::Panel^ graphPanel;
	protected:
	private: System::Windows::Forms::Panel^ controlsPanel;
	private: System::Windows::Forms::Label^ weightLabel;

	private: System::Windows::Forms::Label^ destinationLabel;

	private: System::Windows::Forms::Label^ sourceLabel;

	private: System::Windows::Forms::Label^ fringeLabel;

	private: System::Windows::Forms::Label^ algorithmLabel;
	private: System::Windows::Forms::NumericUpDown^ sourceInput;


	private: System::Windows::Forms::ComboBox^ fringeCombobx;

	private: System::Windows::Forms::ComboBox^ algorithmCombobx;
private: System::Windows::Forms::NumericUpDown^ weightInput;


	private: System::Windows::Forms::NumericUpDown^ destinationInput;
	private: System::Windows::Forms::Button^ resetBtn;

	private: System::Windows::Forms::Button^ nextBtn;

	private: System::Windows::Forms::Button^ pauseBtn;


	private: System::Windows::Forms::Button^ runButton;

	private: System::Windows::Forms::Button^ addEdgeBtn;

	private: System::Windows::Forms::Button^ addVertexBtn;
	private: System::Windows::Forms::ListBox^ stepListbx;

	private: System::Windows::Forms::Label^ statusLabel;
	private: System::Windows::Forms::Timer^ animationTimer;
	private: System::ComponentModel::IContainer^ components;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->graphPanel = (gcnew System::Windows::Forms::Panel());
			this->controlsPanel = (gcnew System::Windows::Forms::Panel());
			this->stepListbx = (gcnew System::Windows::Forms::ListBox());
			this->statusLabel = (gcnew System::Windows::Forms::Label());
			this->resetBtn = (gcnew System::Windows::Forms::Button());
			this->nextBtn = (gcnew System::Windows::Forms::Button());
			this->pauseBtn = (gcnew System::Windows::Forms::Button());
			this->runButton = (gcnew System::Windows::Forms::Button());
			this->addEdgeBtn = (gcnew System::Windows::Forms::Button());
			this->addVertexBtn = (gcnew System::Windows::Forms::Button());
			this->weightInput = (gcnew System::Windows::Forms::NumericUpDown());
			this->destinationInput = (gcnew System::Windows::Forms::NumericUpDown());
			this->sourceInput = (gcnew System::Windows::Forms::NumericUpDown());
			this->fringeCombobx = (gcnew System::Windows::Forms::ComboBox());
			this->algorithmCombobx = (gcnew System::Windows::Forms::ComboBox());
			this->weightLabel = (gcnew System::Windows::Forms::Label());
			this->destinationLabel = (gcnew System::Windows::Forms::Label());
			this->sourceLabel = (gcnew System::Windows::Forms::Label());
			this->fringeLabel = (gcnew System::Windows::Forms::Label());
			this->algorithmLabel = (gcnew System::Windows::Forms::Label());
			this->animationTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->controlsPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->weightInput))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->destinationInput))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->sourceInput))->BeginInit();
			this->SuspendLayout();
			// 
			// graphPanel
			// 
			this->graphPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->graphPanel->BackColor = System::Drawing::Color::White;
			this->graphPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->graphPanel->Location = System::Drawing::Point(20, 20);
			this->graphPanel->Name = L"graphPanel";
			this->graphPanel->Size = System::Drawing::Size(780, 650);
			this->graphPanel->TabIndex = 0;
			this->graphPanel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &GraphForm::graphPanel_Paint);
			// 
			// controlsPanel
			// 
			this->controlsPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Right));
			this->controlsPanel->BackColor = System::Drawing::Color::White;
			this->controlsPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->controlsPanel->Controls->Add(this->stepListbx);
			this->controlsPanel->Controls->Add(this->statusLabel);
			this->controlsPanel->Controls->Add(this->resetBtn);
			this->controlsPanel->Controls->Add(this->nextBtn);
			this->controlsPanel->Controls->Add(this->pauseBtn);
			this->controlsPanel->Controls->Add(this->runButton);
			this->controlsPanel->Controls->Add(this->addEdgeBtn);
			this->controlsPanel->Controls->Add(this->addVertexBtn);
			this->controlsPanel->Controls->Add(this->weightInput);
			this->controlsPanel->Controls->Add(this->destinationInput);
			this->controlsPanel->Controls->Add(this->sourceInput);
			this->controlsPanel->Controls->Add(this->fringeCombobx);
			this->controlsPanel->Controls->Add(this->algorithmCombobx);
			this->controlsPanel->Controls->Add(this->weightLabel);
			this->controlsPanel->Controls->Add(this->destinationLabel);
			this->controlsPanel->Controls->Add(this->sourceLabel);
			this->controlsPanel->Controls->Add(this->fringeLabel);
			this->controlsPanel->Controls->Add(this->algorithmLabel);
			this->controlsPanel->Location = System::Drawing::Point(841, 20);
			this->controlsPanel->Name = L"controlsPanel";
			this->controlsPanel->Size = System::Drawing::Size(340, 650);
			this->controlsPanel->TabIndex = 0;
			// 
			// stepListbx
			// 
			this->stepListbx->FormattingEnabled = true;
			this->stepListbx->ItemHeight = 16;
			this->stepListbx->Location = System::Drawing::Point(27, 454);
			this->stepListbx->Name = L"stepListbx";
			this->stepListbx->Size = System::Drawing::Size(300, 180);
			this->stepListbx->TabIndex = 17;
			this->stepListbx->SelectedIndexChanged += gcnew System::EventHandler(this, &GraphForm::listBox1_SelectedIndexChanged);
			// 
			// statusLabel
			// 
			this->statusLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->statusLabel->Location = System::Drawing::Point(27, 401);
			this->statusLabel->Name = L"statusLabel";
			this->statusLabel->Size = System::Drawing::Size(300, 50);
			this->statusLabel->TabIndex = 16;
			this->statusLabel->Text = L"Ready";
			// 
			// resetBtn
			// 
			this->resetBtn->Location = System::Drawing::Point(188, 330);
			this->resetBtn->Name = L"resetBtn";
			this->resetBtn->Size = System::Drawing::Size(86, 28);
			this->resetBtn->TabIndex = 15;
			this->resetBtn->Text = L"Reset";
			this->resetBtn->UseVisualStyleBackColor = true;
			this->resetBtn->Click += gcnew System::EventHandler(this, &GraphForm::resetBtn_Click);
			// 
			// nextBtn
			// 
			this->nextBtn->Location = System::Drawing::Point(68, 330);
			this->nextBtn->Name = L"nextBtn";
			this->nextBtn->Size = System::Drawing::Size(86, 28);
			this->nextBtn->TabIndex = 14;
			this->nextBtn->Text = L"Next Step";
			this->nextBtn->UseVisualStyleBackColor = true;
			this->nextBtn->Click += gcnew System::EventHandler(this, &GraphForm::nextBtn_Click);
			// 
			// pauseBtn
			// 
			this->pauseBtn->Location = System::Drawing::Point(188, 296);
			this->pauseBtn->Name = L"pauseBtn";
			this->pauseBtn->Size = System::Drawing::Size(86, 28);
			this->pauseBtn->TabIndex = 13;
			this->pauseBtn->Text = L"Pause";
			this->pauseBtn->UseVisualStyleBackColor = true;
			this->pauseBtn->Click += gcnew System::EventHandler(this, &GraphForm::pauseBtn_Click);
			// 
			// runButton
			// 
			this->runButton->Location = System::Drawing::Point(68, 296);
			this->runButton->Name = L"runButton";
			this->runButton->Size = System::Drawing::Size(86, 28);
			this->runButton->TabIndex = 12;
			this->runButton->Text = L"Run";
			this->runButton->UseVisualStyleBackColor = true;
			this->runButton->Click += gcnew System::EventHandler(this, &GraphForm::runButton_Click);
			// 
			// addEdgeBtn
			// 
			this->addEdgeBtn->Location = System::Drawing::Point(188, 262);
			this->addEdgeBtn->Name = L"addEdgeBtn";
			this->addEdgeBtn->Size = System::Drawing::Size(86, 28);
			this->addEdgeBtn->TabIndex = 11;
			this->addEdgeBtn->Text = L"Add Edge";
			this->addEdgeBtn->UseVisualStyleBackColor = true;
			this->addEdgeBtn->Click += gcnew System::EventHandler(this, &GraphForm::addEdgeBtn_Click);
			// 
			// addVertexBtn
			// 
			this->addVertexBtn->Location = System::Drawing::Point(55, 262);
			this->addVertexBtn->Name = L"addVertexBtn";
			this->addVertexBtn->Size = System::Drawing::Size(115, 28);
			this->addVertexBtn->TabIndex = 10;
			this->addVertexBtn->Text = L"Add Vertex";
			this->addVertexBtn->UseVisualStyleBackColor = true;
			this->addVertexBtn->Click += gcnew System::EventHandler(this, &GraphForm::addVertexBtn_Click);
			// 
			// weightInput
			// 
			this->weightInput->Location = System::Drawing::Point(111, 216);
			this->weightInput->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->weightInput->Name = L"weightInput";
			this->weightInput->Size = System::Drawing::Size(120, 22);
			this->weightInput->TabIndex = 9;
			this->weightInput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			// 
			// destinationInput
			// 
			this->destinationInput->Location = System::Drawing::Point(112, 171);
			this->destinationInput->Name = L"destinationInput";
			this->destinationInput->Size = System::Drawing::Size(120, 22);
			this->destinationInput->TabIndex = 8;
			// 
			// sourceInput
			// 
			this->sourceInput->Location = System::Drawing::Point(112, 124);
			this->sourceInput->Name = L"sourceInput";
			this->sourceInput->Size = System::Drawing::Size(120, 22);
			this->sourceInput->TabIndex = 7;
			// 
			// fringeCombobx
			// 
			this->fringeCombobx->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->fringeCombobx->FormattingEnabled = true;
			this->fringeCombobx->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Binary Heap", L"Linked List" });
			this->fringeCombobx->Location = System::Drawing::Point(112, 79);
			this->fringeCombobx->Name = L"fringeCombobx";
			this->fringeCombobx->Size = System::Drawing::Size(121, 24);
			this->fringeCombobx->TabIndex = 6;
			// 
			// algorithmCombobx
			// 
			this->algorithmCombobx->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->algorithmCombobx->FormattingEnabled = true;
			this->algorithmCombobx->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Dijkstra", L"Prim" });
			this->algorithmCombobx->Location = System::Drawing::Point(111, 33);
			this->algorithmCombobx->Name = L"algorithmCombobx";
			this->algorithmCombobx->Size = System::Drawing::Size(121, 24);
			this->algorithmCombobx->TabIndex = 5;
			// 
			// weightLabel
			// 
			this->weightLabel->AutoSize = true;
			this->weightLabel->Location = System::Drawing::Point(126, 197);
			this->weightLabel->Name = L"weightLabel";
			this->weightLabel->Size = System::Drawing::Size(85, 16);
			this->weightLabel->TabIndex = 4;
			this->weightLabel->Text = L"Edge Weight";
			// 
			// destinationLabel
			// 
			this->destinationLabel->AutoSize = true;
			this->destinationLabel->Location = System::Drawing::Point(116, 152);
			this->destinationLabel->Name = L"destinationLabel";
			this->destinationLabel->Size = System::Drawing::Size(115, 16);
			this->destinationLabel->TabIndex = 3;
			this->destinationLabel->Text = L"Destination Vertex";
			// 
			// sourceLabel
			// 
			this->sourceLabel->AutoSize = true;
			this->sourceLabel->Location = System::Drawing::Point(120, 105);
			this->sourceLabel->Name = L"sourceLabel";
			this->sourceLabel->Size = System::Drawing::Size(91, 16);
			this->sourceLabel->TabIndex = 2;
			this->sourceLabel->Text = L"Source Vertex";
			// 
			// fringeLabel
			// 
			this->fringeLabel->AutoSize = true;
			this->fringeLabel->Location = System::Drawing::Point(148, 60);
			this->fringeLabel->Name = L"fringeLabel";
			this->fringeLabel->Size = System::Drawing::Size(45, 16);
			this->fringeLabel->TabIndex = 1;
			this->fringeLabel->Text = L"Fringe";
			// 
			// algorithmLabel
			// 
			this->algorithmLabel->AutoSize = true;
			this->algorithmLabel->Location = System::Drawing::Point(139, 14);
			this->algorithmLabel->Name = L"algorithmLabel";
			this->algorithmLabel->Size = System::Drawing::Size(63, 16);
			this->algorithmLabel->TabIndex = 0;
			this->algorithmLabel->Text = L"Algorithm";
			// 
			// animationTimer
			// 
			this->animationTimer->Interval = 800;
			this->animationTimer->Tick += gcnew System::EventHandler(this, &GraphForm::animationTimer_Tick);
			// 
			// GraphForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1182, 703);
			this->Controls->Add(this->controlsPanel);
			this->Controls->Add(this->graphPanel);
			this->Name = L"GraphForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Graph Algorithm Visualizer";
			this->Load += gcnew System::EventHandler(this, &GraphForm::GraphForm_Load);
			this->controlsPanel->ResumeLayout(false);
			this->controlsPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->weightInput))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->destinationInput))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->sourceInput))->EndInit();
			this->ResumeLayout(false);

		}


private:
	void PrepareDijkstraAnimation(int* distances)
	{
		// Set the current animation algorithm to "Dijkstra" and clear any previous animation data.
		currentAnimationAlgorithm = "Dijkstra";
		mstEdgeSources->Clear();
		mstEdgeDestinations->Clear();


		primParent = nullptr;
		primKey = nullptr;
		primTotalWeight = 0;

		// Stop the animation timer and clear previous animation data.
		animationTimer->Stop();
		animationOrder->Clear();
		animatedVisited->Clear();
		stepListbx->Items->Clear();

		animationDistances =
			gcnew array<int>(vertexPositions->Count);

		array<bool>^ added =
			gcnew array<bool>(vertexPositions->Count);

		// Copy the final distances into managed storage.
		for (int vertex = 0;
			vertex < vertexPositions->Count;
			vertex++)
		{
			animationDistances[vertex] = distances[vertex];
			added[vertex] = false;
		}

		// Arrange vertices from smallest distance to largest.
		for (int count = 0;
			count < vertexPositions->Count;
			count++)
		{
			int minimumVertex = -1;
			int minimumDistance = INT_MAX;

			for (int vertex = 0;
				vertex < vertexPositions->Count;
				vertex++)
			{
				if (!added[vertex] &&
					animationDistances[vertex] < minimumDistance)
				{
					minimumDistance =
						animationDistances[vertex];

					minimumVertex = vertex;
				}
			}

			// Remaining vertices are unreachable.
			if (minimumVertex == -1)
			{
				break;
			}

			animationOrder->Add(minimumVertex);
			added[minimumVertex] = true;
		}

		animationIndex = 0;
		currentAnimatedVertex = -1;

		graphPanel->Invalidate();
	}

	// Prepare the animation for Prim's algorithm.
	private:
		void PreparePrimAnimation(int* parent, int* key, std::vector<int>& visitOrder) {
			//stop any animation that is already running
			animationTimer->Stop();

			//clear information from the previous animation
			animationOrder->Clear();
			animatedVisited->Clear();
			mstEdgeSources->Clear();
			mstEdgeDestinations->Clear();
			stepListbx->Items->Clear();

			int vertexCount = vertexPositions->Count;

			//create managed arrays that the windows form can use
			primParent = gcnew array<int>(vertexCount);
			primKey = gcnew array<int>(vertexCount);

			primTotalWeight = 0;

			// Copy the results from the C++ Prim algorithm
			// into arrays owned by the windows form
			for (int vertex = 0; vertex < vertexCount; vertex++) {
				primParent[vertex] = parent[vertex];
				primKey[vertex] = key[vertex];

				// The starting vertex has parent -1,
				// so it is not included in the total weight
				if (parent[vertex] != -1 && key[vertex] != INT_MAX) {
					primTotalWeight += key[vertex];
				}
			}

			//copy the order in which prim visited the vertices.
			for (int index = 0; index < static_cast<int>(visitOrder.size()); index++) {
				animationOrder->Add(visitOrder[index]);
			}

			//begin the animation at its first step
				animationIndex = 0;
				currentAnimatedVertex = -1;

				//tell the shared timer that it is animating prim
				currentAnimationAlgorithm = "Prim";

				//redraw the graph in its starting state
				graphPanel->Invalidate();
		}


private:
	void ShowNextAnimationStep()
	{
		// Turn the previous gold vertex green.
		if (currentAnimatedVertex != -1 &&
			!animatedVisited->Contains(currentAnimatedVertex))
		{
			animatedVisited->Add(currentAnimatedVertex);
		}

		// stop when every animation step has been displayed
		if (animationIndex >= animationOrder->Count)
		{
			animationTimer->Stop();
			currentAnimatedVertex = -1;
			pauseBtn->Text = "Pause";

			// Display the correct completion message
			if (currentAnimationAlgorithm == "Prim")
			{
				statusLabel->Text =
					"Prim's algorithm animation completed. Total MST weight: " +
					primTotalWeight.ToString();
			}
			else 
			{
				statusLabel->Text =
					"Dijkstra animation completed.";
			}
			graphPanel->Invalidate();
			return;
		}

		//select the next vertex in the recorded order
		currentAnimatedVertex =
			animationOrder[animationIndex];

		if (currentAnimationAlgorithm == "Prim")
		{
			int parent = primParent[currentAnimatedVertex];

			//the source is the first vertec and has no parent
			if (parent == -1)
			{
				stepListbx->Items->Add(
					"Iteration " +
					(animationIndex + 1).ToString() +
					": start at vertex " +
					currentAnimatedVertex.ToString());

				statusLabel->Text = "starting Prim at vertex " +
					currentAnimatedVertex.ToString();		
			}
			else 
			{
				//retreive the weight of the selected MST eedge
				int weight = primKey[currentAnimatedVertex];

				//save the edge so graphPanel_Paint can highlight it
				mstEdgeSources->Add(parent);
				mstEdgeDestinations->Add(currentAnimatedVertex);

				stepListbx->Items->Add(
					"Iteration " +
					(animationIndex + 1).ToString() +
					": add edge " +
					parent.ToString() +
					"-" +
					currentAnimatedVertex.ToString() +
					", Weight " +
					weight.ToString());

				statusLabel->Text =
					"Adding MST edge " +
					parent.ToString() +
					"-" +
					currentAnimatedVertex.ToString();
			}
		}
		else 
		{
			//dijkstra displayed the finalized shortest distance
			int distance = animationDistances[currentAnimatedVertex];

			stepListbx->Items->Add(
				"Iteration " +
				(animationIndex + 1).ToString() +
				": Vertex " +
				currentAnimatedVertex.ToString() +
				", Distance " +
				distance.ToString());

			statusLabel->Text = 
				"current vertex: " +
				currentAnimatedVertex.ToString() +
				" | Distance: " + 
				distance.ToString();
		}

		//move to the next step in the animation order
		animationIndex++;

		//cause graphPanel_Paint to run again and redraw the graph with the new current vertex
		graphPanel->Invalidate();
	}

#pragma endregion
	private: System::Void GraphForm_Load(System::Object^ sender, System::EventArgs^ e) {
		this->Size = System::Drawing::Size(1200, 750);
		algorithmCombobx->SelectedIndex = 0; //dijkstra
		fringeCombobx->SelectedIndex = 0; //binary heap
	}
	private: System::Void listBox1_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	}
private: System::Void animationTimer_Tick(System::Object^ sender, System::EventArgs^ e) {
	ShowNextAnimationStep();
}

private: System::Void graphPanel_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e)
{
	e->Graphics->SmoothingMode =
		System::Drawing::Drawing2D::SmoothingMode::AntiAlias;

	// Draw all edges first.
	for (int edge = 0;
		edge < edgeSources->Count;
		edge++)
	{
		DrawEdge(
			e->Graphics,
			edgeSources[edge],
			edgeDestinations[edge],
			edgeWeights[edge]);
	}

	//draw the MST edges selected so far
	for (int edge = 0;
		edge < mstEdgeSources->Count;
		edge++)
	{
		DrawMSTEdge(
			e->Graphics,
			mstEdgeSources[edge],
			mstEdgeDestinations[edge]);
	}

	// Draw vertices over the edges.
	//colors 
	for (int vertex = 0;
		vertex < vertexPositions->Count;
		vertex++)
	{
		System::Drawing::Color vertexColor =
			System::Drawing::Color::LightGray;

		// Previously visited vertex.
		if (animatedVisited->Contains(vertex))
		{
			vertexColor =
				System::Drawing::Color::LightGreen;
		}

		// Current vertex takes priority.
		if (vertex == currentAnimatedVertex)
		{
			vertexColor =
				System::Drawing::Color::Gold;
		}

		DrawVertex(
			e->Graphics,
			vertex,
			vertexColor);
	}
}
private: System::Void addVertexBtn_Click(System::Object^ sender, System::EventArgs^ e) {
	int vertexID = vertexPositions->Count;

	int column = vertexID % 4;
	int row = vertexID / 4;

	int x = 100 + (column * 180);
	int y = 100 + (row * 150);

	graphModel->createVertex(); // Add vertex to the graph model
	vertexPositions->Add(System::Drawing::Point(x, y));

	statusLabel->Text =
		"Vertex " + vertexID.ToString() + " added.";

	graphPanel->Invalidate();
}

private: System::Void addEdgeBtn_Click(System::Object^ sender, System::EventArgs^ e) {
	int source = Convert::ToInt32(sourceInput->Value);
	int destination = Convert::ToInt32(destinationInput->Value);

	int weight = Convert::ToInt32(weightInput->Value);

	if (source >= vertexPositions->Count ||
		destination >= vertexPositions->Count)
	{
		MessageBox::Show(
			"Both vertices must exist before adding an edge.",
			"Invalid Edge");

		return;
	}

	if (source == destination)
	{
		MessageBox::Show(
			"An edge must connect two different vertices.",
			"Invalid Edge");

		return;
	}

	for (int edge = 0; edge < edgeSources->Count; edge++)
	{
		bool sameDirection =
			edgeSources[edge] == source &&
			edgeDestinations[edge] == destination;

		bool oppositeDirection =
			edgeSources[edge] == destination &&
			edgeDestinations[edge] == source;

		if (sameDirection || oppositeDirection)
		{
			MessageBox::Show(
				"An edge already exists between these vertices.",
				"Duplicate Edge");

			return;
		}
	}

	graphModel->createEdge(source, destination, weight); //add edge to the graph model
	AddVisualEdge(source, destination, weight);

	statusLabel->Text =
		"Added edge " +
		source.ToString() +
		" - " +
		destination.ToString() +
		" with weight " +
		weight.ToString();

	graphPanel->Invalidate();
}

private: System::Void runButton_Click(System::Object^ sender, System::EventArgs^ e) {
	int source = Convert::ToInt32(sourceInput->Value);

	if (source >= vertexPositions->Count)
	{
		MessageBox::Show(
			"The selected source vertex does not exist.",
			"Invalid Source");

		return;
	}

	if (algorithmCombobx->SelectedItem == nullptr ||
		fringeCombobx->SelectedItem == nullptr)
	{
		MessageBox::Show(
			"Please select an algorithm and fringe.",
			"Missing Selection");

		return;
	}

	String^ algorithm =
		algorithmCombobx->SelectedItem->ToString();

	String^ fringe =
		fringeCombobx->SelectedItem->ToString();

	stepListbx->Items->Clear();

	if (algorithm == "Dijkstra")
	{
		int* distances = nullptr;

		if (fringe == "Binary Heap")
		{
			distances =
				graphModel->dijkstrasBinaryHeap(source);
		}
		else
		{
			distances =
				graphModel->dijkstrasLL(source);
		}

		if (distances == nullptr)
		{
			statusLabel->Text =
				"Dijkstra could not run.";

			return;
		}

		//copies the distances into animationDistances
		PrepareDijkstraAnimation(distances);

		//deletes the og native array exactly once
		delete[] distances;
		distances = nullptr;

		statusLabel->Text =
			"Dijkstra animation started using " +
			fringe + ".";

		pauseBtn->Text = "Pause";
		animationTimer->Start();
	}
	else
	{
		//allocate arrays for prims results
		int* parent = new int[graphModel->adjSize];

		int* key = new int[graphModel->adjSize];

		//records the real order produced by prims
		std::vector<int> visitOrder;

		bool connected;

		//run the selected fringe implementation
		if (fringe == "Binary Heap")
		{
			connected =
				graphModel->primsBinaryHeap(source, parent, key, visitOrder);
		}
		else
		{
			connected =
				graphModel->primsLL(source, parent, key, visitOrder);
		}

		//copy the results before deleting the native arrays
		PreparePrimAnimation(parent, key, visitOrder);

		//delete the native arrays
		delete[] parent;
		delete[] key;

		parent = nullptr;
		key = nullptr;

		pauseBtn->Text = "Pause";

		statusLabel->Text =
			"Prim animation started using " +
			fringe + ".";

		animationTimer->Start();

		//prim requires a connected graph for one complete MST
		if (!connected) 
		{
			MessageBox::Show(
				"The graph is not fully connected "
				"Prim can only display the connected portion "
				"Disconnected Graph ");
			
		}
	}

}
private: System::Void pauseBtn_Click(System::Object^ sender, System::EventArgs^ e) {
	if (animationOrder->Count == 0)
	{
		statusLabel->Text =
			"Run an algorithm first.";

		return;
	}

	if (animationTimer->Enabled)
	{
		animationTimer->Stop();
		pauseBtn->Text = "Resume";
		statusLabel->Text = "Animation paused.";
	}
	else if (animationIndex < animationOrder->Count)
	{
		animationTimer->Start();
		pauseBtn->Text = "Pause";
		statusLabel->Text = "Animation resumed.";
	}
}
private: System::Void nextBtn_Click(System::Object^ sender, System::EventArgs^ e) {
	if (animationOrder->Count == 0)
	{
		statusLabel->Text =
			"Run an algorithm first.";

		return;
	}

	animationTimer->Stop();
	pauseBtn->Text = "Resume";

	ShowNextAnimationStep();
}
private: System::Void resetBtn_Click(System::Object^ sender, System::EventArgs^ e) {
	animationTimer->Stop();

    animationOrder->Clear();
    animatedVisited->Clear();

	mstEdgeSources->Clear();
	mstEdgeDestinations->Clear();

    animationDistances = nullptr;
	primParent = nullptr;
	primKey = nullptr;

    animationIndex = 0;
    currentAnimatedVertex = -1;
	primTotalWeight = 0;
	currentAnimationAlgorithm = "";

    stepListbx->Items->Clear();

    pauseBtn->Text = "Pause";
    statusLabel->Text = "Ready";

    graphPanel->Invalidate();
}
};
}
