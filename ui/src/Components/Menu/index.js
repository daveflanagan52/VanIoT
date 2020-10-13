import React from 'react';
import './Menu.scss';

import MenuButton from '../MenuButton';

export default props => {
    let buttons = props.pages.map(
        (page, index) => 
            <MenuButton 
                data={page} 
                selected={props.selected === index} 
                key={index} 
                index={index}
                onClick={() => props.onPageClick(index)}
                />
    );
    return (
        <div className='menu'>
            {buttons}
        </div>
    );
};