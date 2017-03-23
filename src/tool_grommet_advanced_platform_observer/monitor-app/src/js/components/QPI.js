import React, { Component, PropTypes } from 'react';
import Box from 'grommet/components/Box';



export default class QPI extends Component {

  constructor () {
    super();
    this.state = {
    };
  }

  render () {

    var styles = {
      arrow_right_1: {
        left:   (0*(25))+"%", backgroundColor: "#99f7ff"
      },
      arrow_right_2: {
        left:   (1*(25))+"%", backgroundColor: "#36a2eb"
      },
      arrow_right_3: {
        left:   (2*(25))+"%", backgroundColor: "#99f7ff"
      },
      arrow_right_4: {
        left:   (3*(25))+"%", backgroundColor: "#36a2eb"
      },
      arrow_left_1: {
        right:   (0*(25))+"%", backgroundColor: "#99f7ff"
      },
      arrow_left_2: {
        right:   (1*(25))+"%", backgroundColor: "#36a2eb"
      },
      arrow_left_3: {
        right:   (2*(25))+"%", backgroundColor: "#99f7ff"
      },
      arrow_left_4: {
        right:   (3*(25))+"%", backgroundColor: "#36a2eb"
      }
    }


    return (
      <Box direction="row" colorIndex="accent-2"  >

        <div className="qpi-container">

          <div className="qpi-counter-max-right"><div className="title-qpi">Max </div><div className="number-qpi"> </div><div className="unit-qpi">GB/s </div> </div>
          <div className="qpi-counter-avg-right"><div className="title-qpi">Avg </div><div className="number-qpi"> </div><div className="unit-qpi">GB/s </div> </div>
          <div className="qpi-counter-max-left"> <div className="title-qpi">Max </div><div className="number-qpi"> </div><div className="unit-qpi">GB/s </div> </div>
          <div className="qpi-counter-avg-left"> <div className="title-qpi">Avg </div><div className="number-qpi"> </div><div className="unit-qpi">GB/s </div> </div>

          <div className="qpi-arrow-container">
            <img src={require('./pictures/new-white-arrow-right.png')} alt="arrow" className="icon-qpi-arrow-right" />
            <img src={require('./pictures/new-white-arrow-left.png')}    alt="arrow" className="icon-qpi-arrow-left" />
            <img src={require('./pictures/switch.png')}  alt="Switch" className="icon-switch" />
            <div className="qpi-stick-container qpi-stick-container-right">
              <div className="qpi-echelle">
                <div className="qpi-echelle-number-1" style={styles.arrow_right_1}> </div>
                <div className="qpi-echelle-number-2" style={styles.arrow_right_2}> </div>
                <div className="qpi-echelle-number-3" style={styles.arrow_right_3}> </div>
                <div className="qpi-echelle-number-4" style={styles.arrow_right_4}> </div>
              </div>
              <div className="qpi-stick qpi-stick-right"> </div>
              <div className="qpi-line qpi-line-right"  > </div>
            </div>

            <div className="qpi-stick-container qpi-stick-container-left   ">
              <div className="qpi-echelle">
                <div className="qpi-echelle-number-1" style={styles.arrow_left_1}> </div>
                <div className="qpi-echelle-number-2" style={styles.arrow_left_2}> </div>
                <div className="qpi-echelle-number-3" style={styles.arrow_left_3}> </div>
                <div className="qpi-echelle-number-4" style={styles.arrow_left_4}> </div>
              </div>
              <div className="qpi-stick qpi-stick-left"> </div>
              <div className="qpi-line qpi-line-left"  > </div>
            </div>
          </div>
        </div>

      </Box>
    );
  }
};
