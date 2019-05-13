 /* 0, 201828013229064, lujie */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "GraphLite.h"

#define VERTEX_CLASS_NAME(name) SSSP##name

#define INF 0x3F3F3F3F
char* m_v0_id;

class VERTEX_CLASS_NAME(InputFormatter): public InputFormatter {
public:
    int64_t getVertexNum() {
        unsigned long long n;
        sscanf(m_ptotal_vertex_line, "%lld", &n);
        m_total_vertex= n;
        return m_total_vertex;
    }
    int64_t getEdgeNum() {
        unsigned long long n;
        sscanf(m_ptotal_edge_line, "%lld", &n);
        m_total_edge= n;
        return m_total_edge;
    }
    int getVertexValueSize() {
        m_n_value_size = sizeof(int);
        return m_n_value_size;
    }
    int getEdgeValueSize() {
        m_e_value_size = sizeof(int);
        return m_e_value_size;
    }
    int getMessageValueSize() {
        m_m_value_size = sizeof(int);
        return m_m_value_size;
    }
    void loadGraph() {
        unsigned long long last_vertex;
        unsigned long long from;
        unsigned long long to;
        int weight = 0; 
		int value = INF;
        int outdegree = 0;
        
        const char *line = getEdgeLine();

        // Note: modify this if an edge weight is to be read
        //       modify the 'weight' variable

        sscanf(line, "%lld %lld %d", &from, &to, &weight);
        addEdge(from, to, &weight);
	
        last_vertex = from;
        ++outdegree;
        for (int64_t i = 1; i < m_total_edge; ++i) {
            line = getEdgeLine();

            // Note: modify this if an edge weight is to be read
            //       modify the 'weight' variable

            sscanf(line, "%lld %lld %d", &from, &to, &weight);
            if (last_vertex != from) {
                addVertex(last_vertex, &value, outdegree);
                last_vertex = from;
                outdegree = 1;
            } else {
                ++outdegree;
            }
            addEdge(from, to, &weight);
        }
        addVertex(last_vertex, &value, outdegree);
    }
};

class VERTEX_CLASS_NAME(OutputFormatter): public OutputFormatter {
public:
    void writeResult() {
        int64_t vid;
        int value;
        char s[1024];

        for (ResultIterator r_iter; ! r_iter.done(); r_iter.next() ) {
            r_iter.getIdValue(vid, &value);
            int n = sprintf(s, "%lld: %d\n", (unsigned long long)vid, value);
            writeNextResLine(s, n);
        }
    }
};

class VERTEX_CLASS_NAME(): public Vertex <int, int, int> {
public:
    void compute(MessageIterator* pmsgs) {
        int val;
		int len = INF;
		// initialization the vertex value at first time
        if (getSuperstep() == 0) {
			// v0 value is 0
			if (getVertexId() == atoi(m_v0_id)) {
				val = 0;
			}
        } else { 
			// get the minilize path length value
        	for ( ; !pmsgs->done(); pmsgs->next()) {
        		if (len > pmsgs->getValue()) {
        			len = pmsgs->getValue();
            	}
        	}
			// get the vertex value
        	int vertex_val = getValue();
			// if current vertex value is samller
        	if (vertex_val < len) {
            	voteToHalt(); 
				return;
        	}
			val = len;
      	}
		
		// Mutate vertex value to message propagation
        * mutableValue() = val;
		// Get an out-edge iterator
        OutEdgeIterator outEdge = getOutEdgeIterator();
		// send updated msgs to the target node
		for ( ; !outEdge.done(); outEdge.next()) {
			sendMessageTo(outEdge.target(), val + outEdge.getValue());
		}
    }
};

class VERTEX_CLASS_NAME(Graph): public Graph {
public:
    // argv[0]: Hw2Part2.so
    // argv[1]: <input path>
    // argv[2]: <output path>
	// argv[3]: <v0 id>
    void init(int argc, char* argv[]) {

        setNumHosts(5);
        setHost(0, "localhost", 1411);
        setHost(1, "localhost", 1421);
        setHost(2, "localhost", 1431);
        setHost(3, "localhost", 1441);
        setHost(4, "localhost", 1451);

        if (argc < 4) {
           printf ("Usage: %s <input path> <output path> <v0 id>\n", argv[0]);
           exit(1);
        }

        m_pin_path = argv[1];
        m_pout_path = argv[2];
		m_v0_id = argv[3];
    }
};

/* STOP: do not change the code below. */
extern "C" Graph* create_graph() {
    Graph* pgraph = new VERTEX_CLASS_NAME(Graph);

    pgraph->m_pin_formatter = new VERTEX_CLASS_NAME(InputFormatter);
    pgraph->m_pout_formatter = new VERTEX_CLASS_NAME(OutputFormatter);
    pgraph->m_pver_base = new VERTEX_CLASS_NAME();

    return pgraph;
}

extern "C" void destroy_graph(Graph* pobject) {
    delete ( VERTEX_CLASS_NAME()* )(pobject->m_pver_base);
    delete ( VERTEX_CLASS_NAME(OutputFormatter)* )(pobject->m_pout_formatter);
    delete ( VERTEX_CLASS_NAME(InputFormatter)* )(pobject->m_pin_formatter);
    delete ( VERTEX_CLASS_NAME(Graph)* )pobject;
}
