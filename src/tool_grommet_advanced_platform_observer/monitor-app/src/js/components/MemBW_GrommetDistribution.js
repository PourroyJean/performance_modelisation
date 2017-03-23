import React, { Component, PropTypes } from 'react';
import Distribution from 'grommet/components/Distribution';
import Box from 'grommet/components/Box';



export default class MemBW_GrommetDistribution extends Component {

  constructor () {
    super();
    this.state = {
    };
  }

  render () {
    return (
      <Box colorIndex="warning"  className="membw-container" id={"membw_socket_" + this.props.numero} >
        <div className="membw-arrow-container">;
          <img src={require('./pictures/white-ram.png')}  alt="RAM" className="icon-ram" />
        </div>
        <div style={{width: 60+"%", height: 40+"%" }}>
          <Distribution
          size='small'
          series={[{"label": "First", "value": 40, "colorIndex": "graph-1"},
            {"label": "Third", "value": 20, "colorIndex": "unset"},
            {"label": "Fourth", "value": 10, "colorIndex": "graph-2"}]}
           />
        </div>
      </Box>
    );
  }
};


MemBW_GrommetDistribution.propTypes = {
  numero: PropTypes.number.isRequired
}
