export default props => (
  <span>
    <style jsx>
      {`
        @keyframes blink-animation {
          to {
            opacity: 0.3;
          }
        }

        span {
          opacity: 1;
          animation: blink-animation 0.5s linear alternate infinite;
        }
      `}
    </style>
    {props.children}
  </span>
)
